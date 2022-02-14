#include <xboot.h>
#include "gx6605s-video.h"

static void videoParams_Reset(struct gx6605s_hdmi_params *params)
{
    params->mHdmi = 0;
    params->mEncodingOut = YCC444;
    params->mEncodingIn = YCC444;
    params->mColorResolution = 8;
    params->mPixelRepetitionFactor = 0;
    params->mRgbQuantizationRange = 0;
    params->mPixelPackingDefaultPhase = 0;
    params->mColorimetry = 0;
    params->mScanInfo = 0;
    params->mActiveFormatAspectRatio = 8;
    params->mNonUniformScaling = 0;
    params->mExtColorimetry = ~0;
    params->mItContent = 0;
    params->mEndTopBar = ~0;
    params->mStartBottomBar = ~0;
    params->mEndLeftBar = ~0;
    params->mStartRightBar = ~0;
    params->mCscFilter = 0;
    params->mHdmiVideoFormat = 0;
    params->m3dStructure = 0;
    params->m3dExtData = 0;
    params->mHdmiVic = 0;
}

static void videoParams_SetEncodingIn(struct gx6605s_hdmi_params *params, enum gx6605s_hdmi_encoding value)
{
    params->mEncodingIn = value;
}

static void videoParams_SetEncodingOut(struct gx6605s_hdmi_params *params, enum gx6605s_hdmi_encoding value)
{
    params->mEncodingOut = value;
}

static void videoParams_SetColorResolution(struct gx6605s_hdmi_params *params, uint8_t value)
{
    params->mColorResolution = value;
}

static void videoParams_SetDtd(struct gx6605s_hdmi_params *params, struct gx6605s_hdmi_dtd *dtd)
{
    params->mDtd = *dtd;
}

static void videoParams_SetHdmi(struct gx6605s_hdmi_params *params, uint8_t value)
{
    params->mHdmi = value;
}

static void videoParams_SetColorimetry(struct gx6605s_hdmi_params *params, uint8_t value)
{
    params->mColorimetry = value;
}

static int dtd_Fill(struct gx6605s_hdmi_dtd *dtd, uint8_t code)
{
    dtd->mCode = code;
    dtd->mHBorder = 0;
    dtd->mVBorder = 0;
    dtd->mPixelRepetitionInput = 0;
    dtd->mHImageSize = 16;
    dtd->mVImageSize = 9;

    switch (code)
    {
    case 2: /* 720x480p @ 59.94/60Hz 4:3 */
        dtd->mHImageSize = 4;
        dtd->mVImageSize = 3;
        dtd->mHActive = 720;
        dtd->mVActive = 480;
        dtd->mHBlanking = 138;
        dtd->mVBlanking = 45;
        dtd->mHSyncOffset = 16;
        dtd->mVSyncOffset = 9;
        dtd->mHSyncPulseWidth = 62;
        dtd->mVSyncPulseWidth = 6;
        dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
        dtd->mInterlaced = 0;
        dtd->mPixelClock = 2700;
        break;
    case 4: /* 1280x720p @ 59.94/60Hz 16:9 */
        dtd->mHActive = 1280;
        dtd->mVActive = 720;
        dtd->mHBlanking = 370;
        dtd->mVBlanking = 30;
        dtd->mHSyncOffset = 110;
        dtd->mVSyncOffset = 5;
        dtd->mHSyncPulseWidth = 40;
        dtd->mVSyncPulseWidth = 5;
        dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
        dtd->mInterlaced = 0;
        dtd->mPixelClock = 7425;
        break;
    case 5: /* 1920x1080i @ 59.94/60Hz 16:9 */
        dtd->mHActive = 1920;
        dtd->mVActive = 540;
        dtd->mHBlanking = 280;
        dtd->mVBlanking = 22;
        dtd->mHSyncOffset = 88;
        dtd->mVSyncOffset = 2;
        dtd->mHSyncPulseWidth = 44;
        dtd->mVSyncPulseWidth = 5;
        dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
        dtd->mInterlaced = 1;
        dtd->mPixelClock = 7425;
        break;
    case 6: /* 720(1440)x480i @ 59.94/60Hz 4:3 */
        dtd->mHImageSize = 4;
        dtd->mVImageSize = 3;
        dtd->mHActive = 1440;
        dtd->mVActive = 240;
        dtd->mHBlanking = 276;
        dtd->mVBlanking = 22;
        dtd->mHSyncOffset = 38;
        dtd->mVSyncOffset = 4;
        dtd->mHSyncPulseWidth = 124;
        dtd->mVSyncPulseWidth = 3;
        dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
        dtd->mInterlaced = 1;
        dtd->mPixelClock = 2700;
        dtd->mPixelRepetitionInput = 1;
        break;
    case 16: /* 1920x1080p @ 59.94/60Hz 16:9 */
        dtd->mHActive = 1920;
        dtd->mVActive = 1080;
        dtd->mHBlanking = 280;
        dtd->mVBlanking = 45;
        dtd->mHSyncOffset = 88;
        dtd->mVSyncOffset = 4;
        dtd->mHSyncPulseWidth = 44;
        dtd->mVSyncPulseWidth = 5;
        dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
        dtd->mInterlaced = 0;
        dtd->mPixelClock = 14850;
        break;
    case 17: /* 720x576p @ 50Hz 4:3 */
        dtd->mHImageSize = 4;
        dtd->mVImageSize = 3;
        dtd->mHActive = 720;
        dtd->mVActive = 576;
        dtd->mHBlanking = 144;
        dtd->mVBlanking = 49;
        dtd->mHSyncOffset = 12;
        dtd->mVSyncOffset = 5;
        dtd->mHSyncPulseWidth = 64;
        dtd->mVSyncPulseWidth = 5;
        dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
        dtd->mInterlaced = 0;
        dtd->mPixelClock = 2700;
        break;
    case 19: /* 1280x720p @ 50Hz 16:9 */
        dtd->mHActive = 1280;
        dtd->mVActive = 720;
        dtd->mHBlanking = 700;
        dtd->mVBlanking = 30;
        dtd->mHSyncOffset = 440;
        dtd->mVSyncOffset = 5;
        dtd->mHSyncPulseWidth = 40;
        dtd->mVSyncPulseWidth = 5;
        dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
        dtd->mInterlaced = 0;
        dtd->mPixelClock = 7425;
        break;
    case 20: /* 1920x1080i @ 50Hz 16:9 */
        dtd->mHActive = 1920;
        dtd->mVActive = 540;
        dtd->mHBlanking = 720;
        dtd->mVBlanking = 22;
        dtd->mHSyncOffset = 528;
        dtd->mVSyncOffset = 2;
        dtd->mHSyncPulseWidth = 44;
        dtd->mVSyncPulseWidth = 5;
        dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
        dtd->mInterlaced = 1;
        dtd->mPixelClock = 7425;
        break;
    case 21: /* 720(1440)x576i @ 50Hz 4:3 */
        dtd->mHImageSize = 4;
        dtd->mVImageSize = 3;
        dtd->mHActive = 1440;
        dtd->mVActive = 288;
        dtd->mHBlanking = 288;
        dtd->mVBlanking = 24;
        dtd->mHSyncOffset = 24;
        //dtd->mVSyncOffset = 2;
        dtd->mVSyncOffset = 1;
        dtd->mHSyncPulseWidth = 126;
        dtd->mVSyncPulseWidth = 3;
        dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
        dtd->mInterlaced = 1;
        dtd->mPixelClock = 2700;
        dtd->mPixelRepetitionInput = 1;
        break;
    case 31: /* 1920x1080p @ 50Hz 16:9 */
        dtd->mHActive = 1920;
        dtd->mVActive = 1080;
        dtd->mHBlanking = 720;
        dtd->mVBlanking = 45;
        dtd->mHSyncOffset = 528;
        dtd->mVSyncOffset = 4;
        dtd->mHSyncPulseWidth = 44;
        dtd->mVSyncPulseWidth = 5;
        dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
        dtd->mInterlaced = 0;
        dtd->mPixelClock = 14850;
        break;
    default:
        dtd->mCode = -1;
        return FALSE;
    }
    return TRUE;
}

static uint8_t *access_mBaseAddr = 0;
static uint8_t access_Read(uint16_t addr)
{
    uint32_t x= (uint32_t)(addr<< 2) + 0xa4f00000;
    return access_mBaseAddr[x];
}

static void access_Write(uint8_t data, uint16_t addr)
{
    access_mBaseAddr[(uint32_t)(addr<<2) + 0xa4f00000] = data;
}

static void access_CoreWrite(uint8_t data, uint16_t addr, uint8_t shift, uint8_t width)
{
    uint8_t temp = 0;
    uint8_t mask = 0;
    if (width <= 0) {
        return;
    }
    mask = BIT(width) - 1;
    if (data > mask) {
        return;
    }
    temp = access_Read(addr);
    temp &= ~(mask << shift);
    temp |= (data & mask) << shift;
    access_Write(temp, addr);
}

static void access_CoreWriteByte(uint8_t data, uint16_t addr)
{
    access_Write(data, addr);
}

static uint8_t videoParams_GetHdmi(struct gx6605s_hdmi_params *params)
{
    return params->mHdmi;
}

static uint16_t dtd_GetPixelRepetitionInput(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mPixelRepetitionInput;
}

static int videoParams_IsPixelRepetition(struct gx6605s_hdmi_params *params)
{
    return params->mPixelRepetitionFactor > 0 || dtd_GetPixelRepetitionInput(
            &(params->mDtd)) > 0;
}

/* register offsets */
#define FC_DBGFORCE  0x00
#define FC_DBGTMDS0  0x19
#define FC_DBGTMDS1  0x1A
#define FC_DBGTMDS2  0x1B
static void halFrameComposerDebug_ForceAudio(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, baseAddr + FC_DBGFORCE, 4, 1);
}

static void halFrameComposerDebug_ForceVideo(uint16_t baseAddr, uint8_t bit)
{
    /* avoid glitches */
    if (bit != 0)
    {
        access_CoreWriteByte(bit ? 0x00 : 0x00, baseAddr + FC_DBGTMDS2); /* R */
        access_CoreWriteByte(bit ? 0x00 : 0x00, baseAddr + FC_DBGTMDS1); /* G */
        access_CoreWriteByte(bit ? 0xFF : 0x00, baseAddr + FC_DBGTMDS0); /* B */
        access_CoreWrite(bit, baseAddr + FC_DBGFORCE, 0, 1);
    }
    else
    {
        access_CoreWrite(bit, baseAddr + FC_DBGFORCE, 0, 1);
        access_CoreWriteByte(bit ? 0x00 : 0x00, baseAddr + FC_DBGTMDS2); /* R */
        access_CoreWriteByte(bit ? 0x00 : 0x00, baseAddr + FC_DBGTMDS1); /* G */
        access_CoreWriteByte(bit ? 0xFF : 0x00, baseAddr + FC_DBGTMDS0); /* B */
    }
}

#define TX_BASE_ADDR  0x0200
#define VP_BASE_ADDR  0x0800
#define FC_BASE_ADDR  0x1000
#define MC_BASE_ADDR  0x4000
#define CSC_BASE_ADDR  0x4100
static int video_ForceOutput(uint16_t baseAddr, uint8_t force)
{
    halFrameComposerDebug_ForceAudio(baseAddr + FC_BASE_ADDR + 0x0200,
            0);
    halFrameComposerDebug_ForceVideo(baseAddr + FC_BASE_ADDR + 0x0200,
            force);
    return TRUE;
}


/* register offsets */
#define FC_INVIDCONF  0x00
#define FC_INHACTV0  0x01
#define FC_INHACTV1  0x02
#define FC_INHBLANK0  0x03
#define FC_INHBLANK1  0x04
#define FC_INVACTV0  0x05
#define FC_INVACTV1  0x06
#define FC_INVBLANK  0x07
#define FC_HSYNCINDELAY0  0x08
#define FC_HSYNCINDELAY1  0x09
#define FC_HSYNCINWIDTH0  0x0A
#define FC_HSYNCINWIDTH1  0x0B
#define FC_VSYNCINDELAY  0x0C
#define FC_VSYNCINWIDTH  0x0D
#define FC_CTRLDUR  0x11
#define FC_EXCTRLDUR  0x12
#define FC_EXCTRLSPAC  0x13
#define FC_CH0PREAM  0x14
#define FC_CH1PREAM  0x15
#define FC_CH2PREAM  0x16
#define FC_PRCONF  0xE0

static void halFrameComposerVideo_HdcpKeepout(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 7, 1);
}

static void halFrameComposerVideo_VSyncPolarity(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 6, 1);
}

static uint8_t dtd_GetVSyncPolarity(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mVSyncPolarity;
}

static void halFrameComposerVideo_HSyncPolarity(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 5, 1);
}

static void halFrameComposerVideo_DataEnablePolarity(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 4, 1);
}

static void halFrameComposerVideo_DviOrHdmi(uint16_t baseAddr, uint8_t bit)
{
    /* 1: HDMI; 0: DVI */
    access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 3, 1);
}

static void halFrameComposerVideo_VBlankOsc(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 1, 1);
}

static void halFrameComposerVideo_Interlaced(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + FC_INVIDCONF), 0, 1);
}

static uint8_t dtd_GetInterlaced(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mInterlaced;
}

static void halFrameComposerVideo_VActive(uint16_t baseAddr, uint16_t value)
{
    /* 11-bit width */
    access_CoreWriteByte((uint8_t) (value), (baseAddr + FC_INVACTV0));
    access_CoreWrite((uint8_t) (value >> 8), (baseAddr + FC_INVACTV1), 0, 4);
}

static uint16_t dtd_GetVActive(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mVActive;
}

static uint16_t dtd_GetVBlanking(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mVBlanking;
}

static void halFrameComposerVideo_HActive(uint16_t baseAddr, uint16_t value)
{
    /* 12-bit width */
    access_CoreWriteByte((uint8_t) (value), (baseAddr + FC_INHACTV0));
    access_CoreWrite((uint8_t) (value >> 8), (baseAddr + FC_INHACTV1), 0, 5);
}

static void halFrameComposerVideo_HBlank(uint16_t baseAddr, uint16_t value)
{
    /* 10-bit width */
    access_CoreWriteByte((uint8_t) (value), (baseAddr + FC_INHBLANK0));
    access_CoreWrite((uint8_t) (value >> 8), (baseAddr + FC_INHBLANK1), 0, 5);
}

static void halFrameComposerVideo_VBlank(uint16_t baseAddr, uint16_t value)
{
    /* 8-bit width */
    access_CoreWriteByte((uint8_t) (value), (baseAddr + FC_INVBLANK));
}

static void halFrameComposerVideo_HSyncEdgeDelay(uint16_t baseAddr, uint16_t value)
{
    /* 11-bit width */
    access_CoreWriteByte((uint8_t) (value), (baseAddr + FC_HSYNCINDELAY0));
    access_CoreWrite((uint8_t) (value >> 8), (baseAddr + FC_HSYNCINDELAY1), 0, 4);
}

static uint16_t dtd_GetHSyncOffset(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mHSyncOffset;
}

static void halFrameComposerVideo_HSyncPulseWidth(uint16_t baseAddr, uint16_t value)
{
    /* 9-bit width */
    access_CoreWriteByte((uint8_t) (value), (baseAddr + FC_HSYNCINWIDTH0));
    access_CoreWrite((uint8_t) (value >> 8), (baseAddr + FC_HSYNCINWIDTH1), 0, 1);
}

static uint16_t dtd_GetHSyncPulseWidth(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mHSyncPulseWidth;
}

static void halFrameComposerVideo_VSyncEdgeDelay(uint16_t baseAddr, uint16_t value)
{
    /* 8-bit width */
    access_CoreWriteByte((uint8_t) (value), (baseAddr + FC_VSYNCINDELAY));
}

static uint16_t dtd_GetVSyncOffset(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mVSyncOffset;
}

static void halFrameComposerVideo_VSyncPulseWidth(uint16_t baseAddr, uint16_t value)
{
    access_CoreWrite((uint8_t) (value), (baseAddr + FC_VSYNCINWIDTH), 0, 6);
}

static uint16_t dtd_GetVSyncPulseWidth(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mVSyncPulseWidth;
}

static void halFrameComposerVideo_ControlPeriodMinDuration(uint16_t baseAddr, uint8_t value)
{
    access_CoreWriteByte(value, (baseAddr + FC_CTRLDUR));
}

static void halFrameComposerVideo_ExtendedControlPeriodMinDuration(uint16_t baseAddr,
        uint8_t value)
{
    access_CoreWriteByte(value, (baseAddr + FC_EXCTRLDUR));
}

static void halFrameComposerVideo_ExtendedControlPeriodMaxSpacing(uint16_t baseAddr,
        uint8_t value)
{
    access_CoreWriteByte(value, (baseAddr + FC_EXCTRLSPAC));
}

static void halFrameComposerVideo_PreambleFilter(uint16_t baseAddr, uint8_t value,
        unsigned channel)
{
    if (channel == 0)
        access_CoreWriteByte(value, (baseAddr + FC_CH0PREAM));
    else if (channel == 1)
        access_CoreWrite(value, (baseAddr + FC_CH1PREAM), 0, 6);
    else if (channel == 2)
        access_CoreWrite(value, (baseAddr + FC_CH2PREAM), 0, 6);
    else
        ;//LOG("invalid channel number: %d\r\n", channel);
}

static void halFrameComposerVideo_PixelRepetitionInput(uint16_t baseAddr, uint8_t value)
{
    access_CoreWrite(value, (baseAddr + FC_PRCONF), 4, 4);
}

static struct gx6605s_hdmi_dtd* videoParams_GetDtd(struct gx6605s_hdmi_params *params)
{
    return &(params->mDtd);
}

static uint8_t dtd_GetHSyncPolarity(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mHSyncPolarity;
}

static uint16_t dtd_GetHActive(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mHActive;
}

static uint16_t dtd_GetHBlanking(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mHBlanking;
}

static int video_FrameComposer(uint16_t baseAddr, struct gx6605s_hdmi_params *params, uint8_t dataEnablePolarity, uint8_t hdcp)
{
    const struct gx6605s_hdmi_dtd *dtd = videoParams_GetDtd(params);
    uint16_t i = 0;

    /* HDCP support was checked previously */
    halFrameComposerVideo_HdcpKeepout(baseAddr + FC_BASE_ADDR, hdcp);
    halFrameComposerVideo_VSyncPolarity(baseAddr + FC_BASE_ADDR,
            dtd_GetVSyncPolarity(dtd));
    halFrameComposerVideo_HSyncPolarity(baseAddr + FC_BASE_ADDR,
            dtd_GetHSyncPolarity(dtd));
    halFrameComposerVideo_DataEnablePolarity(baseAddr + FC_BASE_ADDR,
            dataEnablePolarity);
    halFrameComposerVideo_DviOrHdmi(baseAddr + FC_BASE_ADDR,
            videoParams_GetHdmi(params));
    halFrameComposerVideo_VBlankOsc(baseAddr + FC_BASE_ADDR,
            dtd_GetInterlaced(dtd));
    halFrameComposerVideo_Interlaced(baseAddr + FC_BASE_ADDR,
            dtd_GetInterlaced(dtd));
    halFrameComposerVideo_VActive(baseAddr + FC_BASE_ADDR,
            dtd_GetVActive(dtd));
    halFrameComposerVideo_HActive(baseAddr + FC_BASE_ADDR,
            dtd_GetHActive(dtd));
    halFrameComposerVideo_HBlank(baseAddr + FC_BASE_ADDR,
            dtd_GetHBlanking(dtd));
    halFrameComposerVideo_VBlank(baseAddr + FC_BASE_ADDR,
            dtd_GetVBlanking(dtd));
    halFrameComposerVideo_HSyncEdgeDelay(baseAddr + FC_BASE_ADDR,
            dtd_GetHSyncOffset(dtd));
    halFrameComposerVideo_HSyncPulseWidth(baseAddr + FC_BASE_ADDR,
            dtd_GetHSyncPulseWidth(dtd));
    halFrameComposerVideo_VSyncEdgeDelay(baseAddr + FC_BASE_ADDR,
            dtd_GetVSyncOffset(dtd));
    halFrameComposerVideo_VSyncPulseWidth(baseAddr + FC_BASE_ADDR,
            dtd_GetVSyncPulseWidth(dtd));
    halFrameComposerVideo_ControlPeriodMinDuration(baseAddr
            + FC_BASE_ADDR, 12);
    halFrameComposerVideo_ExtendedControlPeriodMinDuration(baseAddr
            + FC_BASE_ADDR, 32);
    /* spacing < 256^2 * config / tmdsClock, spacing <= 50ms
     * worst case: tmdsClock == 25MHz => config <= 19
     */
    halFrameComposerVideo_ExtendedControlPeriodMaxSpacing(baseAddr
            + FC_BASE_ADDR, 1);
    for (i = 0; i < 3; i++)
        halFrameComposerVideo_PreambleFilter(baseAddr + FC_BASE_ADDR,
                (i + 1) * 11, i);
    halFrameComposerVideo_PixelRepetitionInput(
            baseAddr + FC_BASE_ADDR, dtd_GetPixelRepetitionInput(dtd)
                    + 1);
    return TRUE;
}

static enum gx6605s_hdmi_encoding videoParams_GetEncodingOut(struct gx6605s_hdmi_params *params)
{
    return params->mEncodingOut;
}

static uint8_t videoParams_GetColorResolution(struct gx6605s_hdmi_params *params)
{
    return params->mColorResolution;
}

/* register offsets */
#define VP_PR_CD  0x01
#define VP_STUFF  0x02
#define VP_REMAP  0x03
#define VP_CONF  0x04

static void halVideoPacketizer_PixelRepetitionFactor(uint16_t baseAddr, uint8_t value)
{
    /* desired factor */
    access_CoreWrite(value, (baseAddr + VP_PR_CD), 0, 4);
    /* enable stuffing */
    access_CoreWrite(1, (baseAddr + VP_STUFF), 0, 1);
    /* enable block */
    access_CoreWrite((value > 1) ? 1 : 0, (baseAddr + VP_CONF), 4, 1);
    /* bypass block */
    access_CoreWrite((value > 1) ? 0 : 1, (baseAddr + VP_CONF), 2, 1);
}

static uint8_t videoParams_GetPixelRepetitionFactor(struct gx6605s_hdmi_params *params)
{
    return params->mPixelRepetitionFactor;
}

static void halVideoPacketizer_ColorDepth(uint16_t baseAddr, uint8_t value)
{
    /* color depth */
    access_CoreWrite(value, (baseAddr + VP_PR_CD), 4, 4);
}

static void halVideoPacketizer_PixelPackingDefaultPhase(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + VP_STUFF), 5, 1);
}

static uint8_t videoParams_GetPixelPackingDefaultPhase(struct gx6605s_hdmi_params *params)
{
    return params->mPixelPackingDefaultPhase;
}

static void halVideoPacketizer_Ycc422RemapSize(uint16_t baseAddr, uint8_t value)
{
    access_CoreWrite(value, (baseAddr + VP_REMAP), 0, 2);
}

static void halVideoPacketizer_OutputSelector(uint16_t baseAddr, uint8_t value)
{
    if (value == 0)
    { /* pixel packing */
        access_CoreWrite(0, (baseAddr + VP_CONF), 6, 1);
        /* enable pixel packing */
        access_CoreWrite(1, (baseAddr + VP_CONF), 5, 1);
        access_CoreWrite(0, (baseAddr + VP_CONF), 3, 1);
    }
    else if (value == 1)
    { /* YCC422 */
        access_CoreWrite(0, (baseAddr + VP_CONF), 6, 1);
        access_CoreWrite(0, (baseAddr + VP_CONF), 5, 1);
        /* enable YCC422 */
        access_CoreWrite(1, (baseAddr + VP_CONF), 3, 1);
    }
    else if (value == 2 || value == 3)
    { /* bypass */
        /* enable bypass */
        access_CoreWrite(1, (baseAddr + VP_CONF), 6, 1);
        access_CoreWrite(0, (baseAddr + VP_CONF), 5, 1);
        access_CoreWrite(0, (baseAddr + VP_CONF), 3, 1);
    }
    else
    {
        return;
    }

    /* YCC422 stuffing */
    access_CoreWrite(1, (baseAddr + VP_STUFF), 2, 1);
    /* pixel packing stuffing */
    access_CoreWrite(1, (baseAddr + VP_STUFF), 1, 1);

    /* ouput selector */
    access_CoreWrite(value, (baseAddr + VP_CONF), 0, 2);
}
static int video_VideoPacketizer(uint16_t baseAddr, struct gx6605s_hdmi_params *params)
{
    unsigned color_depth = 0;
    unsigned remap_size = 0;
    unsigned output_select = 0;

    if (videoParams_GetEncodingOut(params) == RGB
            || videoParams_GetEncodingOut(params) == YCC444)
    {
        if (videoParams_GetColorResolution(params) == 0)
            output_select = 3;
        else if (videoParams_GetColorResolution(params) == 8)
        {
            color_depth = 4;
            output_select = 3;
        }
        else if (videoParams_GetColorResolution(params) == 10)
            color_depth = 5;
        else if (videoParams_GetColorResolution(params) == 12)
            color_depth = 6;
        else if (videoParams_GetColorResolution(params) == 16)
            color_depth = 7;
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }

    halVideoPacketizer_PixelRepetitionFactor(baseAddr + VP_BASE_ADDR,
            videoParams_GetPixelRepetitionFactor(params));
    halVideoPacketizer_ColorDepth(baseAddr + VP_BASE_ADDR, color_depth);
    halVideoPacketizer_PixelPackingDefaultPhase(baseAddr
            + VP_BASE_ADDR, videoParams_GetPixelPackingDefaultPhase(params));
    halVideoPacketizer_Ycc422RemapSize(baseAddr + VP_BASE_ADDR,
            remap_size);
    halVideoPacketizer_OutputSelector(baseAddr + VP_BASE_ADDR,
            output_select);
    return TRUE;
}

static int videoParams_IsColorSpaceInterpolation(struct gx6605s_hdmi_params *params)
{
    return params->mEncodingIn == YCC422 && (params->mEncodingOut == RGB
            || params->mEncodingOut == YCC444);
}

static uint8_t videoParams_GetCscFilter(struct gx6605s_hdmi_params *params)
{
    return params->mCscFilter;
}

static int videoParams_IsColorSpaceDecimation(struct gx6605s_hdmi_params *params)
{
    return params->mEncodingOut == YCC422 && (params->mEncodingIn == RGB
            || params->mEncodingIn == YCC444);
}

/* Color Space Converter register offsets */
#define CSC_CFG  0x00
#define CSC_SCALE  0x01
#define CSC_COEF_A1_MSB  0x02
#define CSC_COEF_A1_LSB  0x03
#define CSC_COEF_A2_MSB  0x04
#define CSC_COEF_A2_LSB  0x05
#define CSC_COEF_A3_MSB  0x06
#define CSC_COEF_A3_LSB  0x07
#define CSC_COEF_A4_MSB  0x08
#define CSC_COEF_A4_LSB  0x09
#define CSC_COEF_B1_MSB  0x0A
#define CSC_COEF_B1_LSB  0x0B
#define CSC_COEF_B2_MSB  0x0C
#define CSC_COEF_B2_LSB  0x0D
#define CSC_COEF_B3_MSB  0x0E
#define CSC_COEF_B3_LSB  0x0F
#define CSC_COEF_B4_MSB  0x10
#define CSC_COEF_B4_LSB  0x11
#define CSC_COEF_C1_MSB  0x12
#define CSC_COEF_C1_LSB  0x13
#define CSC_COEF_C2_MSB  0x14
#define CSC_COEF_C2_LSB  0x15
#define CSC_COEF_C3_MSB  0x16
#define CSC_COEF_C3_LSB  0x17
#define CSC_COEF_C4_MSB  0x18
#define CSC_COEF_C4_LSB  0x19
static void halColorSpaceConverter_Interpolation(uint16_t baseAddr, uint8_t value)
{
    /* 2-bit width */
    access_CoreWrite(value, baseAddr + CSC_CFG, 4, 2);
}

static void halColorSpaceConverter_Decimation(uint16_t baseAddr, uint8_t value)
{
    /* 2-bit width */
    access_CoreWrite(value, baseAddr + CSC_CFG, 0, 2);
}

static void halColorSpaceConverter_CoefficientA1(uint16_t baseAddr, uint16_t value)
{
    /* 15-bit width */
    access_CoreWriteByte((uint8_t) (value), baseAddr + CSC_COEF_A1_LSB);
    access_CoreWrite((uint8_t) (value >> 8), baseAddr + CSC_COEF_A1_MSB, 0, 7);
}

static void halColorSpaceConverter_CoefficientA2(uint16_t baseAddr, uint16_t value)
{
    /* 15-bit width */
    access_CoreWriteByte((uint8_t) (value), baseAddr + CSC_COEF_A2_LSB);
    access_CoreWrite((uint8_t) (value >> 8), baseAddr + CSC_COEF_A2_MSB, 0, 7);
}

static void halColorSpaceConverter_CoefficientA3(uint16_t baseAddr, uint16_t value)
{
    /* 15-bit width */
    access_CoreWriteByte((uint8_t) (value), baseAddr + CSC_COEF_A3_LSB);
    access_CoreWrite((uint8_t) (value >> 8), baseAddr + CSC_COEF_A3_MSB, 0, 7);
}

static void halColorSpaceConverter_CoefficientA4(uint16_t baseAddr, uint16_t value)
{
    /* 15-bit width */
    access_CoreWriteByte((uint8_t) (value), baseAddr + CSC_COEF_A4_LSB);
    access_CoreWrite((uint8_t) (value >> 8), baseAddr + CSC_COEF_A4_MSB, 0, 7);
}

static void halColorSpaceConverter_CoefficientB1(uint16_t baseAddr, uint16_t value)
{
    /* 15-bit width */
    access_CoreWriteByte((uint8_t) (value), baseAddr + CSC_COEF_B1_LSB);
    access_CoreWrite((uint8_t) (value >> 8), baseAddr + CSC_COEF_B1_MSB, 0, 7);
}

static void halColorSpaceConverter_CoefficientB2(uint16_t baseAddr, uint16_t value)
{
    /* 15-bit width */
    access_CoreWriteByte((uint8_t) (value), baseAddr + CSC_COEF_B2_LSB);
    access_CoreWrite((uint8_t) (value >> 8), baseAddr + CSC_COEF_B2_MSB, 0, 7);
}

static void halColorSpaceConverter_CoefficientB3(uint16_t baseAddr, uint16_t value)
{
    /* 15-bit width */
    access_CoreWriteByte((uint8_t) (value), baseAddr + CSC_COEF_B3_LSB);
    access_CoreWrite((uint8_t) (value >> 8), baseAddr + CSC_COEF_B3_MSB, 0, 7);
}

static void halColorSpaceConverter_CoefficientB4(uint16_t baseAddr, uint16_t value)
{
    /* 15-bit width */
    access_CoreWriteByte((uint8_t) (value), baseAddr + CSC_COEF_B4_LSB);
    access_CoreWrite((uint8_t) (value >> 8), baseAddr + CSC_COEF_B4_MSB, 0, 7);
}

static void halColorSpaceConverter_CoefficientC1(uint16_t baseAddr, uint16_t value)
{
    /* 15-bit width */
    access_CoreWriteByte((uint8_t) (value), baseAddr + CSC_COEF_C1_LSB);
    access_CoreWrite((uint8_t) (value >> 8), baseAddr + CSC_COEF_C1_MSB, 0, 7);
}

static void halColorSpaceConverter_CoefficientC2(uint16_t baseAddr, uint16_t value)
{
    /* 15-bit width */
    access_CoreWriteByte((uint8_t) (value), baseAddr + CSC_COEF_C2_LSB);
    access_CoreWrite((uint8_t) (value >> 8), baseAddr + CSC_COEF_C2_MSB, 0, 7);
}

static void halColorSpaceConverter_CoefficientC3(uint16_t baseAddr, uint16_t value)
{
    /* 15-bit width */
    access_CoreWriteByte((uint8_t) (value), baseAddr + CSC_COEF_C3_LSB);
    access_CoreWrite((uint8_t) (value >> 8), baseAddr + CSC_COEF_C3_MSB, 0, 7);
}

static void halColorSpaceConverter_CoefficientC4(uint16_t baseAddr, uint16_t value)
{
    access_CoreWriteByte((uint8_t) (value), baseAddr + CSC_COEF_C4_LSB);
    access_CoreWrite((uint8_t) (value >> 8), baseAddr + CSC_COEF_C4_MSB, 0, 7);
}

static int videoParams_IsColorSpaceConversion(struct gx6605s_hdmi_params *params)
{
    return params->mEncodingIn != params->mEncodingOut;
}

static void videoParams_UpdateCscCoefficients(struct gx6605s_hdmi_params *params)
{
    uint16_t i = 0;
    if (!videoParams_IsColorSpaceConversion(params))
    {
        for (i = 0; i < 4; i++)
        {
            params->mCscA[i] = 0;
            params->mCscB[i] = 0;
            params->mCscC[i] = 0;
        }
        params->mCscA[0] = 0x2000;
        params->mCscB[1] = 0x2000;
        params->mCscC[2] = 0x2000;
        params->mCscScale = 1;
    }
    else if (videoParams_IsColorSpaceConversion(params) && params->mEncodingOut
            == RGB)
    {
        if (params->mColorimetry == ITU601)
        {
            params->mCscA[0] = 0x2000;
            params->mCscA[1] = 0x6926;
            params->mCscA[2] = 0x74fd;
            params->mCscA[3] = 0x010e;

            params->mCscB[0] = 0x2000;
            params->mCscB[1] = 0x2cdd;
            params->mCscB[2] = 0x0000;
            params->mCscB[3] = 0x7e9a;

            params->mCscC[0] = 0x2000;
            params->mCscC[1] = 0x0000;
            params->mCscC[2] = 0x38b4;
            params->mCscC[3] = 0x7e3b;

            params->mCscScale = 1;
        }
        else if (params->mColorimetry == ITU709)
        {
            params->mCscA[0] = 0x2000;
            params->mCscA[1] = 0x7106;
            params->mCscA[2] = 0x7a02;
            params->mCscA[3] = 0x00a7;

            params->mCscB[0] = 0x2000;
            params->mCscB[1] = 0x3264;
            params->mCscB[2] = 0x0000;
            params->mCscB[3] = 0x7e6d;

            params->mCscC[0] = 0x2000;
            params->mCscC[1] = 0x0000;
            params->mCscC[2] = 0x3b61;
            params->mCscC[3] = 0x7e25;

            params->mCscScale = 1;
        }
    }
    else if (videoParams_IsColorSpaceConversion(params) && params->mEncodingIn
            == RGB)
    {
        if (params->mColorimetry == ITU601)
        {
            params->mCscA[0] = 0x2591;
            params->mCscA[1] = 0x1322;
            params->mCscA[2] = 0x074b;
            params->mCscA[3] = 0x0000;

            params->mCscB[0] = 0x6535;
            params->mCscB[1] = 0x2000;
            params->mCscB[2] = 0x7acc;
            params->mCscB[3] = 0x0200;

            params->mCscC[0] = 0x6acd;
            params->mCscC[1] = 0x7534;
            params->mCscC[2] = 0x2000;
            params->mCscC[3] = 0x0200;

            params->mCscScale = 0;
        }
        else if (params->mColorimetry == ITU709)
        {
            params->mCscA[0] = 0x2dc5;
            params->mCscA[1] = 0x0d9b;
            params->mCscA[2] = 0x049e;
            params->mCscA[3] = 0x0000;

            params->mCscB[0] = 0x62f0;
            params->mCscB[1] = 0x2000;
            params->mCscB[2] = 0x7d11;
            params->mCscB[3] = 0x0200;

            params->mCscC[0] = 0x6756;
            params->mCscC[1] = 0x78ab;
            params->mCscC[2] = 0x2000;
            params->mCscC[3] = 0x0200;

            params->mCscScale = 0;
        }
    }
    /* else use user coefficients */
}
static uint16_t * videoParams_GetCscA(struct gx6605s_hdmi_params *params)
{
    videoParams_UpdateCscCoefficients(params);
    return params->mCscA;
}

static uint16_t * videoParams_GetCscB(struct gx6605s_hdmi_params *params)
{
    videoParams_UpdateCscCoefficients(params);
    return params->mCscB;
}

static uint16_t * videoParams_GetCscC(struct gx6605s_hdmi_params *params)
{
    videoParams_UpdateCscCoefficients(params);
    return params->mCscC;
}

static void halColorSpaceConverter_ScaleFactor(uint16_t baseAddr, uint8_t value)
{
    /* 2-bit width */
    access_CoreWrite(value, baseAddr + CSC_SCALE, 0, 2);
}

static uint16_t videoParams_GetCscScale(struct gx6605s_hdmi_params *params)
{
    return params->mCscScale;
}

static void halColorSpaceConverter_ColorDepth(uint16_t baseAddr, uint8_t value)
{
    /* 4-bit width */
    access_CoreWrite(value, baseAddr + CSC_SCALE, 4, 4);
}

static int video_ColorSpaceConverter(uint16_t baseAddr, struct gx6605s_hdmi_params *params)
{
    unsigned interpolation = 0;
    unsigned decimation = 0;
    unsigned color_depth = 0;

    if (videoParams_IsColorSpaceInterpolation(params))//插值
    {
        if (videoParams_GetCscFilter(params) > 1)
        {
            return FALSE;
        }
        interpolation = 1 + videoParams_GetCscFilter(params);
    }
    else if (videoParams_IsColorSpaceDecimation(params))//抽样
    {
        if (videoParams_GetCscFilter(params) > 2)
        {
            return FALSE;
        }
        decimation = 1 + videoParams_GetCscFilter(params);
    }

    if ((videoParams_GetColorResolution(params) == 8)
            || (videoParams_GetColorResolution(params) == 0))
        color_depth = 4;
    else if (videoParams_GetColorResolution(params) == 10)
        color_depth = 5;
    else if (videoParams_GetColorResolution(params) == 12)
        color_depth = 6;
    else if (videoParams_GetColorResolution(params) == 16)
        color_depth = 7;
    else
    {
        return FALSE;
    }

    halColorSpaceConverter_Interpolation(baseAddr + CSC_BASE_ADDR,
            interpolation);
    halColorSpaceConverter_Decimation(baseAddr + CSC_BASE_ADDR,
            decimation);
    halColorSpaceConverter_CoefficientA1(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscA(params)[0]);
    halColorSpaceConverter_CoefficientA2(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscA(params)[1]);
    halColorSpaceConverter_CoefficientA3(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscA(params)[2]);
    halColorSpaceConverter_CoefficientA4(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscA(params)[3]);
    halColorSpaceConverter_CoefficientB1(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscB(params)[0]);
    halColorSpaceConverter_CoefficientB2(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscB(params)[1]);
    halColorSpaceConverter_CoefficientB3(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscB(params)[2]);
    halColorSpaceConverter_CoefficientB4(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscB(params)[3]);
    halColorSpaceConverter_CoefficientC1(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscC(params)[0]);
    halColorSpaceConverter_CoefficientC2(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscC(params)[1]);
    halColorSpaceConverter_CoefficientC3(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscC(params)[2]);
    halColorSpaceConverter_CoefficientC4(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscC(params)[3]);
    halColorSpaceConverter_ScaleFactor(baseAddr + CSC_BASE_ADDR,
            videoParams_GetCscScale(params));
    halColorSpaceConverter_ColorDepth(baseAddr + CSC_BASE_ADDR,
            color_depth);
    return TRUE;
}

static enum gx6605s_hdmi_encoding videoParams_GetEncodingIn(struct gx6605s_hdmi_params *params)
{
    return params->mEncodingIn;
}

/* register offsets */
#define TX_INVID0  0x00
#define TX_INSTUFFING  0x01
#define TX_GYDATA0  0x02
#define TX_GYDATA1  0x03
#define TX_RCRDATA0  0x04
#define TX_RCRDATA1  0x05
#define TX_BCBDATA0  0x06
#define TX_BCBDATA1  0x07
static void halVideoSampler_InternalDataEnableGenerator(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + TX_INVID0), 7, 1);
}

static void halVideoSampler_VideoMapping(uint16_t baseAddr, uint8_t value)
{
    access_CoreWrite(value, (baseAddr + TX_INVID0), 0, 5);
}

static void halVideoSampler_StuffingGy(uint16_t baseAddr, uint16_t value)
{
    access_CoreWriteByte((uint8_t) (value >> 0), (baseAddr + TX_GYDATA0));
    access_CoreWriteByte((uint8_t) (value >> 8), (baseAddr + TX_GYDATA1));
    access_CoreWrite(1, (baseAddr + TX_INSTUFFING), 0, 1);
}

static void halVideoSampler_StuffingRcr(uint16_t baseAddr, uint16_t value)
{
    access_CoreWriteByte((uint8_t) (value >> 0), (baseAddr + TX_RCRDATA0));
    access_CoreWriteByte((uint8_t) (value >> 8), (baseAddr + TX_RCRDATA1));
    access_CoreWrite(1, (baseAddr + TX_INSTUFFING), 1, 1);
}

static void halVideoSampler_StuffingBcb(uint16_t baseAddr, uint16_t value)
{
    access_CoreWriteByte((uint8_t) (value >> 0), (baseAddr + TX_BCBDATA0));
    access_CoreWriteByte((uint8_t) (value >> 8), (baseAddr + TX_BCBDATA1));
    access_CoreWrite(1, (baseAddr + TX_INSTUFFING), 2, 1);
}
static int video_VideoSampler(uint16_t baseAddr, struct gx6605s_hdmi_params *params)
{
    unsigned map_code = 0;

    if (videoParams_GetEncodingIn(params) == RGB || videoParams_GetEncodingIn(
            params) == YCC444)
    {
        if ((videoParams_GetColorResolution(params) == 8)
                || (videoParams_GetColorResolution(params) == 0))
            map_code = 1;
        else if (videoParams_GetColorResolution(params) == 10)
            map_code = 3;
        else if (videoParams_GetColorResolution(params) == 12)
            map_code = 5;
        else if (videoParams_GetColorResolution(params) == 16)
            map_code = 7;
        else
        {
            return FALSE;
        }
        map_code += (videoParams_GetEncodingIn(params) == YCC444) ? 8 : 0;
    }
    else
    {
        return FALSE;
    }

    halVideoSampler_InternalDataEnableGenerator(baseAddr
            + TX_BASE_ADDR, 0);//使用外部数据使能信息
    halVideoSampler_VideoMapping(baseAddr + TX_BASE_ADDR, map_code);//map_code = 9
    halVideoSampler_StuffingGy(baseAddr + TX_BASE_ADDR, 0);
    halVideoSampler_StuffingRcr(baseAddr + TX_BASE_ADDR, 0);
    halVideoSampler_StuffingBcb(baseAddr + TX_BASE_ADDR, 0);
    return TRUE;
}
static int video_Configure(uint16_t baseAddr, struct gx6605s_hdmi_params *params, uint8_t dataEnablePolarity, uint8_t hdcp)
{

    /* DVI mode does not support pixel repetition */
    if (!videoParams_GetHdmi(params) && videoParams_IsPixelRepetition(params))
        return FALSE;
    if (video_ForceOutput(baseAddr, 1) != TRUE)
        return FALSE;
    if (video_FrameComposer(baseAddr, params, dataEnablePolarity, hdcp) != TRUE)
        return FALSE;
    if (video_VideoPacketizer(baseAddr, params) != TRUE)
        return FALSE;
    if (video_ColorSpaceConverter(baseAddr, params) != TRUE)
        return FALSE;
    if (video_VideoSampler(baseAddr, params) != TRUE)
        return FALSE;
    return TRUE;
}

static uint16_t dtd_GetPixelClock(const struct gx6605s_hdmi_dtd *dtd)
{
    return dtd->mPixelClock;
}

/* [0.01 MHz] */
static uint16_t videoParams_GetPixelClock(struct gx6605s_hdmi_params *params)
{
    return dtd_GetPixelClock(&(params->mDtd));
}

/* register offsets */
#define MC_CLKDIS  0x01
#define MC_FLOWCTRL  0x04
#define MC_PHYRSTZ  0x05
#define MC_HEACPHY_RST  0x07
static void halMainController_VideoFeedThroughOff(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + MC_FLOWCTRL), 0, 1);
}

static void halMainController_PixelClockGate(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 0, 1);
}

static void halMainController_TmdsClockGate(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 1, 1);
}

static void halMainController_PixelRepetitionClockGate(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 2, 1);
}

static void halMainController_ColorSpaceConverterClockGate(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 4, 1);
}

static void halMainController_AudioSamplerClockGate(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 3, 1);
}

static void halMainController_CecClockGate(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 5, 1);
}

static void halMainController_HdcpClockGate(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + MC_CLKDIS), 6, 1);
}

static int control_Configure(uint16_t baseAddr, uint16_t pClk, uint8_t pRep, uint8_t cRes, int cscOn, int audioOn,
        int cecOn, int hdcpOn)
{
    halMainController_VideoFeedThroughOff(baseAddr + MC_BASE_ADDR, cscOn ? 1 : 0);
    /*  clock gate == 0 => turn on modules */
    halMainController_PixelClockGate(baseAddr + MC_BASE_ADDR, 0);
    halMainController_TmdsClockGate(baseAddr + MC_BASE_ADDR, 0);
    halMainController_PixelRepetitionClockGate(baseAddr
            + MC_BASE_ADDR, (pRep > 0) ? 0 : 1);
    halMainController_ColorSpaceConverterClockGate(baseAddr
            + MC_BASE_ADDR, cscOn ? 0 : 1);
    halMainController_AudioSamplerClockGate(baseAddr + MC_BASE_ADDR,
            audioOn ? 0 : 1);
    halMainController_CecClockGate(baseAddr + MC_BASE_ADDR,
            cecOn ? 0 : 1);
    halMainController_HdcpClockGate(baseAddr + MC_BASE_ADDR,
            hdcpOn ? 0 : 1);
    return TRUE;
}

/* register offsets */
#define PHY_CONF0  0x00
#define PHY_TST0  0x01
#define PHY_STAT0  0x04
static void halSourcePhy_Gen2TxPowerOn(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + PHY_CONF0), 3, 1);
}

static void halSourcePhy_Gen2PDDQ(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + PHY_CONF0), 4, 1);
}

static void halMainController_PhyReset(uint16_t baseAddr, uint8_t bit)
{
    /* active low */
    access_CoreWrite(bit ? 0 : 1, (baseAddr + MC_PHYRSTZ), 0, 1);
}

static void halMainController_HeacPhyReset(uint16_t baseAddr, uint8_t bit)
{
    /* active high */
    access_CoreWrite(bit ? 1 : 0, (baseAddr + MC_HEACPHY_RST), 0, 1);
}

static void halSourcePhy_TestClear(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + PHY_TST0), 5, 1);
}

/* register offsets */
#define PHY_I2CM_SLAVE  0x00
#define PHY_I2CM_ADDRESS  0x01
#define PHY_I2CM_DATAO  0x02
#define PHY_I2CM_DATAI  0x04
#define PHY_I2CM_OPERATION  0x06
static void halI2cMasterPhy_SlaveAddress(uint16_t baseAddr, uint8_t value)
{
    access_CoreWrite(value, (baseAddr + PHY_I2CM_SLAVE), 0, 7);
}

static void halI2cMasterPhy_RegisterAddress(uint16_t baseAddr, uint8_t value)
{
    access_CoreWriteByte(value, (baseAddr + PHY_I2CM_ADDRESS));
}

static void halI2cMasterPhy_WriteData(uint16_t baseAddr, uint16_t value)
{
    access_CoreWriteByte((uint8_t) (value >> 8), (baseAddr + PHY_I2CM_DATAO + 0));
    access_CoreWriteByte((uint8_t) (value >> 0), (baseAddr + PHY_I2CM_DATAO + 1));
}

static void halI2cMasterPhy_WriteRequest(uint16_t baseAddr)
{
    access_CoreWrite(1, (baseAddr + PHY_I2CM_OPERATION), 4, 1);
}

#define PHY_BASE_ADDR  0x3000
#define PHY_I2CM_BASE_ADDR  0x3020
#define PHY_I2C_SLAVE_ADDR  0x69
static int phy_I2cWrite(uint16_t baseAddr, uint16_t data, uint8_t addr)
{
    udelay(100);
    halI2cMasterPhy_RegisterAddress(baseAddr + PHY_I2CM_BASE_ADDR, addr);
    halI2cMasterPhy_WriteData(baseAddr + PHY_I2CM_BASE_ADDR, data);
    halI2cMasterPhy_WriteRequest(baseAddr + PHY_I2CM_BASE_ADDR);
    return TRUE;
}

static void halSourcePhy_Gen2EnHpdRxSense(uint16_t baseAddr, uint8_t bit)
{
    access_CoreWrite(bit, (baseAddr + PHY_CONF0), 2, 1);
}

static uint8_t access_CoreReadByte(uint16_t addr)
{
    uint8_t data = 0;
    data = access_Read(addr);
    return data;
}

static uint8_t access_CoreRead(uint16_t addr, uint8_t shift, uint8_t width)
{
    if (width <= 0)
    {
        return 0;
    }
    return (access_CoreReadByte(addr) >> shift) & (BIT(width) - 1);
}
static uint8_t halSourcePhy_PhaseLockLoopState(uint16_t baseAddr)
{
    return access_CoreRead((baseAddr + PHY_STAT0), 0, 1);
}

#define PHY_TIMEOUT (3000*50)
static int phy_Configure (uint16_t baseAddr, uint16_t pClk, uint8_t cRes, uint8_t pRep, uint8_t phy_model)
{
    uint16_t i = 0;
    /*  colour resolution 0 is 8 bit colour depth */
    if (cRes == 0)
        cRes = 8;

    if (pRep != 0)
    {
        return FALSE;
    }

    /* PHY_GEN2_TSMC_40G_1_8V */ /*E102*/
    if(phy_model == 102)
    {
        halSourcePhy_Gen2TxPowerOn(baseAddr + PHY_BASE_ADDR, 0);
        halSourcePhy_Gen2PDDQ(baseAddr + PHY_BASE_ADDR, 1);
        halMainController_PhyReset(baseAddr + MC_BASE_ADDR, 0);
        halMainController_PhyReset(baseAddr + MC_BASE_ADDR, 1);
        halMainController_HeacPhyReset(baseAddr + MC_BASE_ADDR, 1);
        halSourcePhy_TestClear(baseAddr + PHY_BASE_ADDR, 1);
        halI2cMasterPhy_SlaveAddress(baseAddr + PHY_I2CM_BASE_ADDR, PHY_I2C_SLAVE_ADDR);
        halSourcePhy_TestClear(baseAddr + PHY_BASE_ADDR, 0);

        phy_I2cWrite(baseAddr, 0x0070, 0x1e);
        phy_I2cWrite(baseAddr, 0x0800, 0x13);
        phy_I2cWrite(baseAddr, 0x0006, 0x19);
        phy_I2cWrite(baseAddr, 0x0004, 0x17);
        phy_I2cWrite(baseAddr, 0x8000, 0x05);
        switch (pClk)
        {
        case 2700:
            phy_I2cWrite(baseAddr, 0x01e0, 0x06);
            phy_I2cWrite(baseAddr, 0x091c, 0x10);
            phy_I2cWrite(baseAddr, 0x0000, 0x15);
            phy_I2cWrite(baseAddr, 0x8009, 0x09);
            phy_I2cWrite(baseAddr, 0x0210, 0x0E);
            break;
        case 7425:
            phy_I2cWrite(baseAddr, 0x0140, 0x06);
            phy_I2cWrite(baseAddr, 0x06dc, 0x10);
            phy_I2cWrite(baseAddr, 0x0005, 0x15);
            phy_I2cWrite(baseAddr, 0x8009, 0x09);
            phy_I2cWrite(baseAddr, 0x0210, 0x0E);
            break;
        case 14850:
            phy_I2cWrite(baseAddr, 0x00a0, 0x06);
            phy_I2cWrite(baseAddr, 0x06dc, 0x10);
            phy_I2cWrite(baseAddr, 0x000a, 0x15);
            phy_I2cWrite(baseAddr, 0x8009, 0x09);
            phy_I2cWrite(baseAddr, 0x0210, 0x0E);
            break;
        default:
            return FALSE;
        }
        halSourcePhy_Gen2EnHpdRxSense(baseAddr + PHY_BASE_ADDR, 1);
        halSourcePhy_Gen2TxPowerOn(baseAddr + PHY_BASE_ADDR, 1);
        halSourcePhy_Gen2PDDQ(baseAddr + PHY_BASE_ADDR, 0);
    }
    /* wait PHY_TIMEOUT no of cycles at most for the pll lock signal to raise ~around 20us max */
    for (i = 0; i < PHY_TIMEOUT; i++)
    {
        if ((i % 100) == 0)
        {
            if (halSourcePhy_PhaseLockLoopState(baseAddr + PHY_BASE_ADDR) == TRUE)
            {
                break;
            }
        }
    }
    if (halSourcePhy_PhaseLockLoopState(baseAddr + PHY_BASE_ADDR) != TRUE)
    {
        return FALSE;
    }

    return TRUE;
}

static int dw_api_Configure(uint16_t cn_hdmi_base_addr, struct gx6605s_hdmi_params * video, uint8_t phy_model)
{
    int audioOn = 0;
    int hdcpOn = 0;
    uint16_t api_mBaseAddress = cn_hdmi_base_addr;

    if (video_Configure(api_mBaseAddress, video, 1, hdcpOn) != TRUE)
    {
            return FALSE;
    }
    if (control_Configure(api_mBaseAddress, videoParams_GetPixelClock(video),
                videoParams_GetPixelRepetitionFactor(video),
                videoParams_GetColorResolution(video),
                videoParams_IsColorSpaceConversion(video), audioOn, FALSE, hdcpOn)
            != TRUE)
    {
            return FALSE;
    }
    if (phy_Configure(api_mBaseAddress, videoParams_GetPixelClock(video),
                videoParams_GetColorResolution(video),
                videoParams_GetPixelRepetitionFactor(video), phy_model) != TRUE)
    {
            return FALSE;
    }
    /* disable blue screen transmission after turning on all necessary blocks (e.g. HDCP) */
    if (video_ForceOutput(api_mBaseAddress, FALSE) != TRUE)
    {
            return FALSE;
    }
    /* send AVMUTE CLEAR (optional) */
    return TRUE;
}

int dw_hdmi_configure(struct gx6605s_hdmi_params *video, uint16_t ceacode)
{
    struct gx6605s_hdmi_dtd tmp_dtd;

    videoParams_Reset(video);
    videoParams_SetEncodingIn(video, YCC444);
    videoParams_SetEncodingOut(video, RGB);
    videoParams_SetColorResolution(video, 0);

    dtd_Fill(&tmp_dtd, ceacode);
    videoParams_SetDtd(video, &tmp_dtd);
    videoParams_SetHdmi(video, TRUE);
    videoParams_SetColorResolution(video, 8);

    if (ceacode == 2 || ceacode == 16 || ceacode == 21 || ceacode == 17){
        videoParams_SetColorimetry(video, ITU601);
    }else{
        videoParams_SetColorimetry(video, ITU709);
    }

    dw_api_Configure(0,video, 102);

    return TRUE;
}
