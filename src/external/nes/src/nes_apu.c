/*
 * Copyright PeakRacing
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nes.h"

#if (NES_ENABLE_SOUND == 1)

// https://www.nesdev.org/wiki/APU_Length_Counter
static const uint8_t length_counter_table[32] = {
/*       |   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
---------+---------------------------------------------------------------- */
/*00-0F*/  0x0A,0xFE,0x14,0x02,0x28,0x04,0x50,0x06,0xA0,0x08,0x3C,0x0A,0x0E,0x0C,0x1A,0x0E,
/*10-1F*/  0x0C,0x10,0x18,0x12,0x30,0x14,0x60,0x16,0xC0,0x18,0x48,0x1A,0x10,0x1C,0x20,0x1E,
};

static const uint8_t apu_pulse_wave[4][8] = {
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0},
    {1, 0, 0, 1, 1, 1, 1, 1}
};

static const uint8_t apu_triangle_wave[32] = {
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};


static inline void nes_apu_pulse_sweep(pulse_t* pulse, uint8_t period_one){
    if (pulse->sweep_divider == 0 && pulse->enabled && pulse->shift){
        if (pulse->cur_period >= 8 && pulse->cur_period <= 0x7ff){
            if (pulse->negate){
                    pulse->cur_period = pulse->cur_period - (pulse->cur_period >> pulse->shift) - period_one;
            }else{
                pulse->cur_period = pulse->cur_period + (pulse->cur_period >> pulse->shift);
            }
        }
    }
    if (pulse->sweep_reload || (pulse->sweep_divider == 0)){ //扫描单元重新开始
        pulse->sweep_reload = 0;
        pulse->sweep_divider = pulse->period;
    }else{
        pulse->sweep_divider--;
    }
}

static inline void nes_apu_length_counter_and_sweep(nes_t* nes){
    // length_counter
    if (!nes->nes_apu.pulse1.len_counter_halt && nes->nes_apu.pulse1.length_counter){
        nes->nes_apu.pulse1.length_counter--;
    }
    if (!nes->nes_apu.pulse2.len_counter_halt && nes->nes_apu.pulse2.length_counter){
        nes->nes_apu.pulse2.length_counter--;
    }
    if (!nes->nes_apu.triangle.len_counter_halt && nes->nes_apu.triangle.length_counter){
        nes->nes_apu.triangle.length_counter--;
    }
    if (!nes->nes_apu.noise.len_counter_halt && nes->nes_apu.noise.length_counter){
        nes->nes_apu.noise.length_counter--;
    }
    // sweep
    nes_apu_pulse_sweep(&nes->nes_apu.pulse1,1);
    nes_apu_pulse_sweep(&nes->nes_apu.pulse2,0);
}

static inline void nes_apu_pulse_envelopes(pulse_t* pulse){
    if (pulse->envelope_restart){//包络重新开始
        pulse->envelope_restart = 0;
        pulse->envelope_divider = pulse->envelope_lowers;
        pulse->envelope_volume = 15;
    }else{
        if (pulse->envelope_divider){
            pulse->envelope_divider--;
        }else{
            pulse->envelope_divider = pulse->envelope_lowers;
            if (pulse->envelope_volume){
                pulse->envelope_volume--;
            }else{
                if (pulse->len_counter_halt){
                    pulse->envelope_volume = 15;
                }
            }
        }
    }
}

static inline void nes_apu_noise_envelopes(noise_t* noise){
    if (noise->envelope_restart){//包络重新开始
        noise->envelope_restart = 0;
        noise->envelope_divider = noise->volume_envelope;
        noise->envelope_volume = 15;
    }else{
        if (noise->envelope_divider){
            noise->envelope_divider--;
        }else{
            noise->envelope_divider = noise->volume_envelope;
            if (noise->envelope_volume){
                noise->envelope_volume--;
            }else{
                if (noise->len_counter_halt){
                    noise->envelope_volume = 15;
                }
            }
        }
    }
}

static inline void nes_apu_triangle_linear_counter(triangle_t* triangle){
    if (triangle->linear_restart){
        triangle->linear_counter = triangle->linear_counter_load;
    }else if (triangle->linear_counter){
        triangle->linear_counter--;
    }
    if (!triangle->len_counter_halt)
        triangle->linear_restart = 0;
}

static inline void nes_apu_envelopes_and_linear_counter(nes_t *nes){
    nes_apu_pulse_envelopes(&nes->nes_apu.pulse1);
    nes_apu_pulse_envelopes(&nes->nes_apu.pulse2);
    nes_apu_triangle_linear_counter(&nes->nes_apu.triangle);
    nes_apu_noise_envelopes(&nes->nes_apu.noise);
}

// https://www.nesdev.org/wiki/APU_Pulse
static void nes_apu_play_pulse(nes_apu_t* apu,uint8_t pulse_id){
    pulse_t* pulse = (pulse_id==1)? &apu->pulse1: &apu->pulse2;
    uint8_t volume,enabled = (pulse_id==1)? apu->status_pulse1: apu->status_pulse2;
    for (uint64_t sample_local = apu->sample_local_start; sample_local <= apu->sample_local_end; sample_local++){
        uint8_t mute = 0;
        if (pulse->sample_index >= NES_APU_SAMPLE_PER_SYNC){
            break;
        }
        if ((!enabled) || (pulse->length_counter == 0)){
            mute = 1;
            pulse->sample_buffer[pulse->sample_index++] = 0;
            pulse->seq_local_old = 0;
            continue;
        }else if (pulse->cur_period <= 7 || pulse->cur_period >= 0x800)
            mute = 1;
        const uint64_t cpu_local = sample_local* NES_CPU_CLOCK_FREQ / NES_APU_SAMPLE_RATE;
        const uint64_t cpu_local_diff = cpu_local - apu->cpu_lock_count;
        // fpulse = fCPU/(16*(t+1))
        const float seq_diff = cpu_local_diff * 1.0f / (16 * (pulse->cur_period + 1));
        const float seq_local = (seq_diff + pulse->seq_local_old) - (int)(seq_diff + pulse->seq_local_old);
        if (mute){
            volume = 0;
        }else if (pulse->constant_volume){
            volume = pulse->envelope_lowers;
        }else{
            volume = pulse->envelope_volume;
        }
        pulse->sample_buffer[pulse->sample_index++] = apu_pulse_wave[pulse->duty][(int)(seq_local * 8)] * volume;
        if (sample_local == (uint64_t)((apu->clock_count + 1) * NES_APU_SAMPLE_RATE / 240)){
            pulse->seq_local_old = seq_local;
        }
    }
}

// https://www.nesdev.org/wiki/APU_Triangle
static void nes_apu_play_triangle(nes_apu_t* apu){
    triangle_t* triangle = &apu->triangle;
    for (uint64_t sample_local = apu->sample_local_start; sample_local <= apu->sample_local_end; sample_local++){
        if (triangle->sample_index >= NES_APU_SAMPLE_PER_SYNC){
            break;
        }
        if ((!apu->status_triangle) || (triangle->length_counter == 0) || (triangle->linear_counter == 0)){
            triangle->sample_buffer[triangle->sample_index++] = 0;
            triangle->seq_local_old = 0;
            continue;
        }
        const uint64_t cpu_local = sample_local* NES_CPU_CLOCK_FREQ / NES_APU_SAMPLE_RATE;
        const uint64_t cpu_local_diff = cpu_local - apu->cpu_lock_count;
        const float seq_diff = cpu_local_diff * 1.0f / (16 * (triangle->cur_period + 1));
        const float seq_local = (seq_diff + triangle->seq_local_old) - (int)(seq_diff + triangle->seq_local_old);
        uint8_t volume = apu_triangle_wave[(int)(seq_local * 32)];
        triangle->sample_buffer[triangle->sample_index++] = volume;
        if (sample_local == (uint64_t)((apu->clock_count + 1) * NES_APU_SAMPLE_RATE / 240)){
            triangle->seq_local_old = seq_local;
        }
    }
}

// https://www.nesdev.org/wiki/APU_Noise
static void nes_apu_play_noise(nes_apu_t* apu){
    noise_t* noise = &apu->noise;
    uint8_t volume;
    uint64_t cpu_local_old = apu->cpu_lock_count;
    for (uint64_t sample_local = apu->sample_local_start; sample_local <= apu->sample_local_end; sample_local++){
        if (noise->sample_index >= NES_APU_SAMPLE_PER_SYNC){
            break;
        }
        if ((!apu->status_noise) || (noise->length_counter == 0)){
            noise->sample_buffer[noise->sample_index++] = 0;
            continue;
        }
        const uint64_t cpu_local = sample_local* NES_CPU_CLOCK_FREQ / NES_APU_SAMPLE_RATE;
        const uint64_t lfsr_count = (cpu_local / (noise->noise_period + 1)) - (cpu_local_old / (noise->noise_period + 1));
        cpu_local_old = cpu_local;
        if (noise->loop_noise){ //短模式
            for (uint64_t t = 0; t < lfsr_count; t++){
                noise->lfsr = (noise->lfsr >> 1) | ((uint16_t)((noise->lfsr_d0 ^ noise->lfsr_d6) << 14));
            }
        }else{                  //长模式
            for (uint64_t t = 0; t < lfsr_count; t++){
                noise->lfsr = (noise->lfsr >> 1) | ((uint16_t)((noise->lfsr_d0 ^ noise->lfsr_d1) << 14));
            }
        }
        if (noise->constant_volume){
            volume = noise->volume_envelope;
        }else{
            volume = noise->envelope_volume;
        }
        noise->sample_buffer[noise->sample_index++] = (uint8_t)(noise->lfsr_d0 * volume);
    }
}

// https://www.nesdev.org/wiki/APU_DMC
static void nes_apu_play_dmc(nes_apu_t* apu){

    for (uint64_t sample_local = apu->sample_local_start; sample_local <= apu->sample_local_end; sample_local++){

    }
}

static inline void nes_apu_play(nes_t* nes){
    nes->nes_apu.cpu_lock_count = nes->nes_apu.clock_count * NES_CPU_CLOCK_FREQ / 240;
    nes->nes_apu.sample_local_start = nes->nes_apu.clock_count * NES_APU_SAMPLE_RATE / 240 + 1;
    nes->nes_apu.sample_local_end = (nes->nes_apu.clock_count + 1) * NES_APU_SAMPLE_RATE / 240;
    nes_apu_play_pulse(&nes->nes_apu,1);
    nes_apu_play_pulse(&nes->nes_apu,2);
    nes_apu_play_triangle(&nes->nes_apu);
    nes_apu_play_noise(&nes->nes_apu);
    nes_apu_play_dmc(&nes->nes_apu);

    if (nes->nes_apu.clock_count % 4 == 3){
        // https://www.nesdev.org/wiki/APU_Mixer
        nes_memset(nes->nes_apu.sample_buffer, 0, NES_APU_SAMPLE_PER_SYNC);
        for (int t = 0; t <= NES_APU_SAMPLE_PER_SYNC - 1; t++){
            // 这里采用线性近似方法 https://www.nesdev.org/wiki/APU_Mixer#Linear_Approximation
            float volume_total = 0.00752f * (nes->nes_apu.pulse1.sample_buffer[t] + nes->nes_apu.pulse2.sample_buffer[t]);
            volume_total += 0.00851f * nes->nes_apu.triangle.sample_buffer[t] + 0.00494f * nes->nes_apu.noise.sample_buffer[t] + 0.00335f * nes->nes_apu.dmc.sample_buffer[t];
            nes->nes_apu.sample_buffer[t] = (uint8_t)(volume_total * 256);
        }
        nes_memset(nes->nes_apu.pulse1.sample_buffer, 0, NES_APU_SAMPLE_PER_SYNC);
        nes_memset(nes->nes_apu.pulse2.sample_buffer, 0, NES_APU_SAMPLE_PER_SYNC);
        nes_memset(nes->nes_apu.triangle.sample_buffer, 0, NES_APU_SAMPLE_PER_SYNC);
        nes_memset(nes->nes_apu.noise.sample_buffer, 0, NES_APU_SAMPLE_PER_SYNC);
        nes_memset(nes->nes_apu.dmc.sample_buffer, 0, NES_APU_SAMPLE_PER_SYNC);

        nes->nes_apu.pulse1.sample_index = 0;
        nes->nes_apu.pulse2.sample_index = 0;
        nes->nes_apu.triangle.sample_index = 0;
        nes->nes_apu.noise.sample_index = 0;
        nes->nes_apu.dmc.sample_index = 0;

        nes_sound_output(nes->nes_apu.sample_buffer, NES_APU_SAMPLE_PER_SYNC);
    }
}

extern void nes_cpu_irq(nes_t* nes);
static inline void nes_apu_frame_irq(nes_t *nes){
    if (nes->nes_apu.irq_inhibit_flag==0){
        nes->nes_apu.frame_interrupt = 1;
        nes_cpu_irq(nes);
    }
}

/*
https://www.nesdev.org/wiki/APU#Frame_Counter_($4017)
https://www.nesdev.org/wiki/APU_Frame_Counter

mode 0:    mode 1:       function
---------  -----------  -----------------------------
 - - - f    - - - - -    IRQ (if bit 6 is clear)
 - l - l    - l - - l    Length counter and sweep
 e e e e    e e e - e    Envelope and linear counter
*/
void nes_apu_frame(nes_t* nes){
    if(nes->nes_apu.mode){// 5 step mode
        switch(nes->nes_apu.clock_count % 5){
            case 0:
                nes_apu_envelopes_and_linear_counter(nes);
                break;
            case 1:
                nes_apu_length_counter_and_sweep(nes);
                nes_apu_envelopes_and_linear_counter(nes);
                break;
            case 2:
                nes_apu_envelopes_and_linear_counter(nes);
                break;
            case 4:
                nes_apu_length_counter_and_sweep(nes);
                nes_apu_envelopes_and_linear_counter(nes);
                break;
        }
    }else{  // 4 step mode
        switch(nes->nes_apu.clock_count % 4){
            case 0:
                nes_apu_envelopes_and_linear_counter(nes);
                break;
            case 1:
                nes_apu_length_counter_and_sweep(nes);
                nes_apu_envelopes_and_linear_counter(nes);
                break;
            case 2:
                nes_apu_envelopes_and_linear_counter(nes);
                break;
            case 3:
                nes_apu_frame_irq(nes);
                nes_apu_length_counter_and_sweep(nes);
                nes_apu_envelopes_and_linear_counter(nes);
                break;
            default:
                break;
        }
    }
    nes_apu_play(nes);
    nes->nes_apu.clock_count++;
}

void nes_apu_init(nes_t *nes){
    nes->nes_apu.status = 0;
    nes->nes_apu.noise.lfsr = 1;
}

uint8_t nes_read_apu_register(nes_t *nes,uint16_t address){
    uint8_t data = 0;
    if(address==0x4015){
        data=nes->nes_apu.status&0xc0;

        if (nes->nes_apu.pulse1.length_counter) data |= 1;
        if (nes->nes_apu.pulse2.length_counter) data |= (1 << 1);
        if (nes->nes_apu.triangle.length_counter) data |= (1 << 2);
        if (nes->nes_apu.noise.length_counter) data |= (1 << 3);
        if (nes->nes_apu.dmc.load_counter) data |= (1 << 4);

        nes->nes_apu.frame_interrupt = 0;
    }else{
        NES_LOG_DEBUG("nes_read apu %04X %02X\n",address,data);
    }
    return data;
}

void nes_write_apu_register(nes_t* nes,uint16_t address,uint8_t data){
    switch(address){
        // Pulse ($4000–$4007)
        // Pulse0 ($4000–$4003)
        case 0x4000:
            nes->nes_apu.pulse1.control0=data;
            break;
        case 0x4001:
            nes->nes_apu.pulse1.control1=data;
            nes->nes_apu.pulse1.sweep_reload=1;
            break;
        case 0x4002:
            nes->nes_apu.pulse1.timer_low=data;
            nes->nes_apu.pulse1.cur_period=nes->nes_apu.pulse1.timer_high<<8|nes->nes_apu.pulse1.timer_low;
            break;
        case 0x4003:
            nes->nes_apu.pulse1.control3=data;
            if (nes->nes_apu.status_pulse1){
                nes->nes_apu.pulse1.length_counter = length_counter_table[nes->nes_apu.pulse1.len_counter_load];
            }
            nes->nes_apu.pulse1.cur_period=nes->nes_apu.pulse1.timer_high<<8|nes->nes_apu.pulse1.timer_low;
            nes->nes_apu.pulse1.envelope_restart = 1;
            nes->nes_apu.pulse1.seq_local_old = 0;
            break;
        // Pulse1 ($4004–$4007)
        case 0x4004:
            nes->nes_apu.pulse2.control0=data;
            break;
        case 0x4005:
            nes->nes_apu.pulse2.control1=data;
            nes->nes_apu.pulse2.sweep_reload=1;
            break;
        case 0x4006:
            nes->nes_apu.pulse2.timer_low=data;
            break;
        case 0x4007:
            nes->nes_apu.pulse2.control3=data;
            if (nes->nes_apu.status_pulse2){
                nes->nes_apu.pulse2.length_counter = length_counter_table[nes->nes_apu.pulse2.len_counter_load];
            }
            nes->nes_apu.pulse2.cur_period=nes->nes_apu.pulse2.timer_high<<8|nes->nes_apu.pulse2.timer_low;
            nes->nes_apu.pulse2.envelope_restart = 1;
            nes->nes_apu.pulse2.seq_local_old = 0;
            break;
        // Triangle ($4008–$400B)
        case 0x4008:
            nes->nes_apu.triangle.control0=data;
            break;
        // case 0x4009:
        //     break;
        case 0x400A:
            nes->nes_apu.triangle.timer_low=data;
            nes->nes_apu.triangle.cur_period=nes->nes_apu.triangle.timer_high<<8|nes->nes_apu.triangle.timer_low;
            break;
        case 0x400B:
            nes->nes_apu.triangle.control3=data;
            if (nes->nes_apu.status_triangle){
                nes->nes_apu.triangle.length_counter = length_counter_table[nes->nes_apu.triangle.len_counter_load];
            }
            nes->nes_apu.triangle.cur_period=nes->nes_apu.triangle.timer_high<<8|nes->nes_apu.triangle.timer_low;
            nes->nes_apu.triangle.linear_restart = 1;
            break;
        // Noise ($400C–$400F)
        case 0x400C:
            nes->nes_apu.noise.control0=data;
            break;
        // case 0x400D:
        //     break;
        case 0x400E:
            nes->nes_apu.noise.control2=data;
            break;
        case 0x400F:
            nes->nes_apu.noise.control3=data;
            if (nes->nes_apu.status_noise){
                nes->nes_apu.noise.length_counter = length_counter_table[nes->nes_apu.noise.len_counter_load];
            }
            nes->nes_apu.noise.envelope_restart = 1;
            break;
        // DMC ($4010–$4013)
        case 0x4010:
            nes->nes_apu.dmc.control0=data;
            break;
        case 0x4011:
            nes->nes_apu.dmc.control1=data;
            break;
        case 0x4012:
            nes->nes_apu.dmc.sample_address=data;
            break;
        case 0x4013:
            nes->nes_apu.dmc.sample_length=data;
            break;
        // case 0x4014:
        //     break;
        // Status ($4015) https://www.nesdev.org/wiki/APU#Status_($4015)
        case 0x4015:
            nes->nes_apu.status=data;
            if (nes->nes_apu.status_pulse1==0){
                nes->nes_apu.pulse1.length_counter=0;
            }
            if (nes->nes_apu.status_pulse2==0){
                nes->nes_apu.pulse2.length_counter=0;
            }
            if (nes->nes_apu.status_triangle==0){
                nes->nes_apu.triangle.length_counter=0;
            }
            if (nes->nes_apu.status_noise==0){
                nes->nes_apu.noise.length_counter=0;
            }
            // nes->nes_apu.dmc_interrupt = 0;
            break;
        case 0x4017:
            nes->nes_apu.frame_counter=data;
            if (nes->nes_apu.irq_inhibit_flag){
                nes->nes_apu.frame_interrupt = 0;
            }
            if (nes->nes_apu.mode){
                nes_apu_length_counter_and_sweep(nes);
                nes_apu_envelopes_and_linear_counter(nes);
            }
            break;
        default:
            NES_LOG_DEBUG("nes_write apu %04X %02X\n",address,data);
            break;
    }
}

#endif
