/*
 * driver/gnss/gnss.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <gnss/gnss.h>

static ssize_t gnss_write_enable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct gnss_t * nav = (struct gnss_t *)kobj->priv;
	gnss_enable(nav);
	return size;
}

static ssize_t gnss_write_disable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct gnss_t * nav = (struct gnss_t *)kobj->priv;
	gnss_disable(nav);
	return size;
}

struct gnss_t * search_gnss(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_GNSS);
	if(!dev)
		return NULL;
	return (struct gnss_t *)dev->priv;
}

struct gnss_t * search_first_gnss(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_GNSS);
	if(!dev)
		return NULL;
	return (struct gnss_t *)dev->priv;
}

struct device_t * register_gnss(struct gnss_t * nav, struct driver_t * drv)
{
	struct device_t * dev;

	if(!nav || !nav->name || !nav->read)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	dev->name = strdup(nav->name);
	dev->type = DEVICE_TYPE_GNSS;
	dev->driver = drv;
	dev->priv = nav;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "enable", NULL, gnss_write_enable, nav);
	kobj_add_regular(dev->kobj, "disable", NULL, gnss_write_disable, nav);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_gnss(struct gnss_t * nav)
{
	struct device_t * dev;

	if(nav && nav->name)
	{
		dev = search_device(nav->name, DEVICE_TYPE_GNSS);
		if(dev && unregister_device(dev))
		{
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
}

void gnss_enable(struct gnss_t * nav)
{
	if(nav && nav->enable)
		nav->enable(nav);
}

void gnss_disable(struct gnss_t * nav)
{
	if(nav && nav->disable)
		nav->disable(nav);
}

static int hex_to_int(const char * start, const char * end)
{
	int len = end - start;
	int result = 0;
	int c;

	for(; len > 0; len--, start++)
	{
		if(start >= end)
			return 0;
		c = *start;
		if((c >= '0') && (c <= '9'))
			c = c - '0';
		else if((c >= 'A') && (c <= 'F'))
			c = c - 'A' + 10;
		else if((c >= 'a') && (c <= 'f'))
			c = c - 'a' + 10;
		else
			return 0;
		result = result * 16 + c;
	}
	return result;
}

static int str_to_int(const char * start, const char * end)
{
	int len = end - start;
	int result = 0;
	int c;

	for(; len > 0; len--, start++)
	{
		if(start >= end)
			return 0;
		c = *start - '0';
		if(c >= 10)
			return 0;
		result = result * 10 + c;
	}
	return result;
}

static double latitude_longitude_convert(const char * s)
{
	double v = strtod(s, NULL);
	int d = (int)(floor(v) / 100);
	return d + (v - d * 100) / 60.0;
}

static inline void gnss_nmea_parse_gga(struct gnss_nmea_t * nmea)
{
	const char * p;

	if(nmea->tsize > 9)
	{
		if(nmea->tend[1] - nmea->tstart[1] >= 6)
		{
			p = nmea->tstart[1];
			nmea->utc.hour = str_to_int(p, p + 2);
			nmea->utc.minute = str_to_int(p + 2, p + 4);
			nmea->utc.second = str_to_int(p + 4, p + 6);
			switch(nmea->tend[1] - nmea->tstart[1])
			{
			case 8:
				nmea->utc.millisecond = str_to_int(p + 7, p + 8) * 100;
				break;
			case 9:
				nmea->utc.millisecond = str_to_int(p + 7, p + 9) * 10;
				break;
			case 10:
				nmea->utc.millisecond = str_to_int(p + 7, p + 10);
				break;
			default:
				nmea->utc.millisecond = 0;
				break;
			}
		}
		else
		{
			nmea->utc.hour = 0;
			nmea->utc.minute = 0;
			nmea->utc.second = 0;
			nmea->utc.millisecond = 0;
		}

		if(nmea->tend[2] - nmea->tstart[2] >= 6)
		{
			nmea->latitude = latitude_longitude_convert(nmea->tstart[2]);
			if(toupper(*nmea->tstart[3]) == 'S')
				nmea->latitude = -nmea->latitude;
		}
		else
			nmea->latitude = 0;

		if(nmea->tend[4] - nmea->tstart[4] >= 6)
		{
			nmea->longitude  = latitude_longitude_convert(nmea->tstart[4]);
			if(toupper(*nmea->tstart[5]) == 'W')
				nmea->longitude  = -nmea->longitude ;
		}
		else
			nmea->longitude = 0;

		if(nmea->tend[6] - nmea->tstart[6] > 0)
			nmea->signal = str_to_int(nmea->tstart[6], nmea->tend[6]);
		else
			nmea->signal = GNSS_SIGNAL_INVALID;

		if(nmea->tend[7] - nmea->tstart[7] > 0)
			nmea->used = str_to_int(nmea->tstart[7], nmea->tend[7]);
		else
			nmea->used = 0;

		if(nmea->tend[8] - nmea->tstart[8] > 0)
			nmea->precision.hdop = strtod(nmea->tstart[8], NULL);
		else
			nmea->precision.hdop = 0;

		if(nmea->tend[9] - nmea->tstart[9] > 0)
			nmea->altitude = strtod(nmea->tstart[9], NULL);
		else
			nmea->altitude = 0;
	}
}

static inline void gnss_nmea_parse_rmc(struct gnss_nmea_t * nmea)
{
	const char * p;

	if(nmea->tsize > 10)
	{
		if(nmea->tend[1] - nmea->tstart[1] >= 6)
		{
			p = nmea->tstart[1];
			nmea->utc.hour = str_to_int(p, p + 2);
			nmea->utc.minute = str_to_int(p + 2, p + 4);
			nmea->utc.second = str_to_int(p + 4, p + 6);
			switch(nmea->tend[1] - nmea->tstart[1])
			{
			case 8:
				nmea->utc.millisecond = str_to_int(p + 7, p + 8) * 100;
				break;
			case 9:
				nmea->utc.millisecond = str_to_int(p + 7, p + 9) * 10;
				break;
			case 10:
				nmea->utc.millisecond = str_to_int(p + 7, p + 10);
				break;
			default:
				nmea->utc.millisecond = 0;
				break;
			}
		}
		else
		{
			nmea->utc.hour = 0;
			nmea->utc.minute = 0;
			nmea->utc.second = 0;
			nmea->utc.millisecond = 0;
		}

		if(nmea->tend[3] - nmea->tstart[3] >= 6)
		{
			nmea->latitude = latitude_longitude_convert(nmea->tstart[3]);
			if(toupper(*nmea->tstart[4]) == 'S')
				nmea->latitude = -nmea->latitude;
		}
		else
			nmea->latitude = 0;

		if(nmea->tend[5] - nmea->tstart[5] >= 6)
		{
			nmea->longitude  = latitude_longitude_convert(nmea->tstart[5]);
			if(toupper(*nmea->tstart[6]) == 'W')
				nmea->longitude  = -nmea->longitude ;
		}
		else
			nmea->longitude = 0;

		if(nmea->tend[7] - nmea->tstart[7] > 0)
			nmea->speed = strtod(nmea->tstart[7], NULL) * 1.852;
		else
			nmea->speed = 0;

		if(nmea->tend[8] - nmea->tstart[8] > 0)
			nmea->track = strtod(nmea->tstart[8], NULL);
		else
			nmea->track = 0;

		if(nmea->tend[9] - nmea->tstart[9] >= 6)
		{
			p = nmea->tstart[9];
			nmea->utc.day = str_to_int(p, p + 2);
			nmea->utc.month = str_to_int(p + 2, p + 4);
			nmea->utc.year = str_to_int(p + 4, p + 6) + 2000;
		}
		else
		{
			nmea->utc.day = 1;
			nmea->utc.month = 1;
			nmea->utc.year = 2000;
		}

		if(nmea->tend[10] - nmea->tstart[10] > 0)
		{
			nmea->magvar = strtod(nmea->tstart[10], NULL);
			if(toupper(*nmea->tstart[11]) == 'W')
				nmea->magvar = -nmea->magvar ;
		}
		else
			nmea->magvar = 0;
	}
}

static inline void gnss_nmea_parse_vtg(struct gnss_nmea_t * nmea)
{
	if(nmea->tsize > 7)
	{
		if(nmea->tend[1] - nmea->tstart[1] > 0)
			nmea->track = strtod(nmea->tstart[1], NULL);
		else
			nmea->track = 0;

		if(nmea->tend[3] - nmea->tstart[3] > 0)
			nmea->mtrack = strtod(nmea->tstart[3], NULL);
		else
			nmea->mtrack = 0;

		if(nmea->tend[7] - nmea->tstart[7] > 0)
			nmea->speed = strtod(nmea->tstart[7], NULL);
		else
			nmea->speed = 0;
	}
}

static inline void gnss_nmea_parse_gsa(struct gnss_nmea_t * nmea)
{
	if(nmea->tsize > 17)
	{
		if(nmea->tend[2] - nmea->tstart[2] > 0)
			nmea->fix = str_to_int(nmea->tstart[2], nmea->tend[2]);
		else
			nmea->fix = GNSS_FIX_NONE;

		if(nmea->tend[15] - nmea->tstart[15] > 0)
			nmea->precision.pdop = strtod(nmea->tstart[15], NULL);
		else
			nmea->precision.pdop = 0;

		if(nmea->tend[16] - nmea->tstart[16] > 0)
			nmea->precision.hdop = strtod(nmea->tstart[16], NULL);
		else
			nmea->precision.hdop = 0;

		if(nmea->tend[17] - nmea->tstart[17] > 0)
			nmea->precision.vdop = strtod(nmea->tstart[17], NULL);
		else
			nmea->precision.vdop = 0;
	}
}

static inline void gnss_nmea_parse_gpgsv(struct gnss_nmea_t * nmea)
{
	int total, n;
	int viewed, c;
	int i;

	if(nmea->tsize > 3)
	{
		total = str_to_int(nmea->tstart[1], nmea->tend[1]);
		n = str_to_int(nmea->tstart[2], nmea->tend[2]);
		viewed = min((int)str_to_int(nmea->tstart[3], nmea->tend[3]), (int)ARRAY_SIZE(nmea->satellite.gps.sv));
		if(n <= 1)
			c = 0;
		else
			c = (n - 1) * 4;
		for(i = 0; (i < 4) && (c < viewed); i++)
		{
			nmea->satellite.gps.sv[c].prn = str_to_int(nmea->tstart[i * 4 + 4], nmea->tend[i * 4 + 4]);
			nmea->satellite.gps.sv[c].elevation = str_to_int(nmea->tstart[i * 4 + 5], nmea->tend[i * 4 + 5]);
			nmea->satellite.gps.sv[c].azimuth = str_to_int(nmea->tstart[i * 4 + 6], nmea->tend[i * 4 + 6]);
			nmea->satellite.gps.sv[c].snr = str_to_int(nmea->tstart[i * 4 + 7], nmea->tend[i * 4 + 7]);
			c++;
		}
		if(n == total)
		{
			for(i = viewed; i < ARRAY_SIZE(nmea->satellite.gps.sv); i++)
			{
				nmea->satellite.gps.sv[i].prn = 0;
				nmea->satellite.gps.sv[i].elevation = 0;
				nmea->satellite.gps.sv[i].azimuth = 0;
				nmea->satellite.gps.sv[i].snr = 0;
			}
			nmea->satellite.gps.n = viewed;
		}
	}
}

static inline void gnss_nmea_parse_bdgsv(struct gnss_nmea_t * nmea)
{
	int total, n;
	int viewed, c;
	int i;

	if(nmea->tsize > 3)
	{
		total = str_to_int(nmea->tstart[1], nmea->tend[1]);
		n = str_to_int(nmea->tstart[2], nmea->tend[2]);
		viewed = min((int)str_to_int(nmea->tstart[3], nmea->tend[3]), (int)ARRAY_SIZE(nmea->satellite.beidou.sv));
		if(n <= 1)
			c = 0;
		else
			c = (n - 1) * 4;
		for(i = 0; (i < 4) && (c < viewed); i++)
		{
			nmea->satellite.beidou.sv[c].prn = str_to_int(nmea->tstart[i * 4 + 4], nmea->tend[i * 4 + 4]);
			nmea->satellite.beidou.sv[c].elevation = str_to_int(nmea->tstart[i * 4 + 5], nmea->tend[i * 4 + 5]);
			nmea->satellite.beidou.sv[c].azimuth = str_to_int(nmea->tstart[i * 4 + 6], nmea->tend[i * 4 + 6]);
			nmea->satellite.beidou.sv[c].snr = str_to_int(nmea->tstart[i * 4 + 7], nmea->tend[i * 4 + 7]);
			c++;
		}
		if(n == total)
		{
			for(i = viewed; i < ARRAY_SIZE(nmea->satellite.beidou.sv); i++)
			{
				nmea->satellite.beidou.sv[i].prn = 0;
				nmea->satellite.beidou.sv[i].elevation = 0;
				nmea->satellite.beidou.sv[i].azimuth = 0;
				nmea->satellite.beidou.sv[i].snr = 0;
			}
			nmea->satellite.beidou.n = viewed;
		}
	}
}

int gnss_refresh(struct gnss_t * nav)
{
	struct gnss_nmea_t * nmea = NULL;
	unsigned char cksum;
	char buf[256];
	int n, i, j;
	int update = 0;

	if(nav && nav->read)
	{
		nmea = &nav->nmea;
		while((n = nav->read(nav, buf, sizeof(buf))) > 0)
		{
			for(i = 0; i < n; i++)
			{
				switch(buf[i])
				{
				case '$':
					nmea->bindex = 0;
					nmea->dindex = 0;
					nmea->tsize = 0;
					break;

				case ',':
				case '*':
				case '\r':
					nmea->tstart[nmea->tsize] = &nmea->buffer[nmea->bindex - nmea->dindex];
					nmea->tend[nmea->tsize] = &nmea->buffer[nmea->bindex];
					nmea->tsize++;
					nmea->buffer[nmea->bindex] = buf[i];
					nmea->bindex++;
					nmea->dindex = 0;
					break;

				case '\n':
					if(nmea->tsize > 2)
					{
						for(j = 0, cksum = 0; j < nmea->tend[nmea->tsize - 2] - nmea->tstart[0]; j++)
							cksum ^= nmea->buffer[j];
						if(cksum == hex_to_int(nmea->tstart[nmea->tsize - 1], nmea->tend[nmea->tsize - 1]))
						{
							for(j = 0; j < nmea->tsize; j++)
								*nmea->tend[j] = 0;
							switch(shash(nmea->tstart[0]))
							{
							case 0x0d30e1ab: /* "GPGGA" */
							case 0x0ccfd29a: /* "BDGGA" */
							case 0x0d2fc8e9: /* "GNGGA" */
								gnss_nmea_parse_gga(nmea);
								break;

							case 0x0d31113e: /* "GPRMC" */
							case 0x0cd0022d: /* "BDRMC" */
							case 0x0d2ff87c: /* "GNRMC" */
								gnss_nmea_parse_rmc(nmea);
								break;

							case 0x0d31232d: /* "GPVTG" */
							case 0x0cd0141c: /* "BDVTG" */
							case 0x0d300a6b: /* "GNVTG" */
								gnss_nmea_parse_vtg(nmea);
								break;

							case 0x0d30e337: /* "GPGSA" */
							case 0x0ccfd426: /* "BDGSA" */
							case 0x0d2fca75: /* "GNGSA" */
								gnss_nmea_parse_gsa(nmea);
								break;

							case 0x0d30e34c: /* "GPGSV" */
								gnss_nmea_parse_gpgsv(nmea);
								break;

							case 0x0ccfd43b: /* "BDGSV" */
								gnss_nmea_parse_bdgsv(nmea);
								break;

							default:
								break;
							}
							update = 1;
						}
					}
					nmea->bindex = 0;
					nmea->dindex = 0;
					nmea->tsize = 0;
					break;

				default:
					nmea->buffer[nmea->bindex] = buf[i];
					nmea->bindex++;
					nmea->dindex++;
					break;
				}
			}
		}
	}
	return update;
}
