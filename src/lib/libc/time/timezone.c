/*
 * libc/time/timezone.c
 */

#include <shash.h>
#include <time.h>
#include <xboot/module.h>

int timezone(const char * tz)
{
	int o = 0;

	switch(shash(tz))
	{
	case 0x233ca014: /* UTC-11:00 - "Pacific/Niue" */
		o = -39600;
		break;
	case 0x603aebd0: /* UTC-11:00 - "Pacific/Pago_Pago" */
		o = -39600;
		break;
	case 0xe6e70af9: /* UTC-10:00 - "Pacific/Honolulu" */
		o = -36000;
		break;
	case 0x9981a3b0: /* UTC-10:00 - "Pacific/Rarotonga" */
		o = -36000;
		break;
	case 0xf24c2446: /* UTC-10:00 - "Pacific/Tahiti" */
		o = -36000;
		break;
	case 0x57ca7135: /* UTC-09:30 - "Pacific/Marquesas" */
		o = -34200;
		break;
	case 0x5a79260e: /* UTC-09:00 - "America/Anchorage" */
		o = -32400;
		break;
	case 0x53720c3a: /* UTC-09:00 - "Pacific/Gambier" */
		o = -32400;
		break;
	case 0xb7f7e8f2: /* UTC-08:00 - "America/Los_Angeles" */
		o = -28800;
		break;
	case 0x6aa1df72: /* UTC-08:00 - "America/Tijuana" */
		o = -28800;
		break;
	case 0x2c6f6b1f: /* UTC-08:00 - "America/Vancouver" */
		o = -28800;
		break;
	case 0x54e0e3e8: /* UTC-08:00 - "America/Whitehorse" */
		o = -28800;
		break;
	case 0x8837d8bd: /* UTC-08:00 - "Pacific/Pitcairn" */
		o = -28800;
		break;
	case 0x97d10b2a: /* UTC-07:00 - "America/Denver" */
		o = -25200;
		break;
	case 0x34b5af01: /* UTC-07:00 - "America/Phoenix" */
		o = -25200;
		break;
	case 0x0532189e: /* UTC-07:00 - "America/Mazatlan" */
		o = -25200;
		break;
	case 0x6cf24e5b: /* UTC-07:00 - "America/Dawson_Creek" */
		o = -25200;
		break;
	case 0x6cb9484a: /* UTC-07:00 - "America/Edmonton" */
		o = -25200;
		break;
	case 0x065d21c4: /* UTC-07:00 - "America/Hermosillo" */
		o = -25200;
		break;
	case 0x0f76c76f: /* UTC-07:00 - "America/Yellowknife" */
		o = -25200;
		break;
	case 0x93256c81: /* UTC-06:00 - "America/Belize" */
		o = -21600;
		break;
	case 0x4b92b5d4: /* UTC-06:00 - "America/Chicago" */
		o = -21600;
		break;
	case 0xd0d93f43: /* UTC-06:00 - "America/Mexico_City" */
		o = -21600;
		break;
	case 0xb875371c: /* UTC-06:00 - "America/Regina" */
		o = -21600;
		break;
	case 0xbfd6fd4c: /* UTC-06:00 - "America/Tegucigalpa" */
		o = -21600;
		break;
	case 0x8c7dafc7: /* UTC-06:00 - "America/Winnipeg" */
		o = -21600;
		break;
	case 0x63ff66be: /* UTC-06:00 - "America/Costa_Rica" */
		o = -21600;
		break;
	case 0x752ad652: /* UTC-06:00 - "America/El_Salvador" */
		o = -21600;
		break;
	case 0xa952f752: /* UTC-06:00 - "Pacific/Galapagos" */
		o = -21600;
		break;
	case 0x0c8259f7: /* UTC-06:00 - "America/Guatemala" */
		o = -21600;
		break;
	case 0x3d5e7600: /* UTC-06:00 - "America/Managua" */
		o = -21600;
		break;
	case 0x953331be: /* UTC-05:00 - "America/Cancun" */
		o = -18000;
		break;
	case 0x93d7bc62: /* UTC-05:00 - "America/Bogota" */
		o = -18000;
		break;
	case 0xcf54f7e7: /* UTC-05:00 - "Pacific/Easter" */
		o = -18000;
		break;
	case 0x1e2a7654: /* UTC-05:00 - "America/New_York" */
		o = -18000;
		break;
	case 0x2de310bf: /* UTC-05:00 - "America/Iqaluit" */
		o = -18000;
		break;
	case 0x792e851b: /* UTC-05:00 - "America/Toronto" */
		o = -18000;
		break;
	case 0x17e64958: /* UTC-05:00 - "America/Guayaquil" */
		o = -18000;
		break;
	case 0xa0e15675: /* UTC-05:00 - "America/Havana" */
		o = -18000;
		break;
	case 0x565dad6c: /* UTC-05:00 - "America/Jamaica" */
		o = -18000;
		break;
	case 0x980468c9: /* UTC-05:00 - "America/Lima" */
		o = -18000;
		break;
	case 0xaedef011: /* UTC-05:00 - "America/Nassau" */
		o = -18000;
		break;
	case 0xb3863854: /* UTC-05:00 - "America/Panama" */
		o = -18000;
		break;
	case 0x8e4a7bdc: /* UTC-05:00 - "America/Port-au-Prince" */
		o = -18000;
		break;
	case 0x9d352764: /* UTC-05:00 - "America/Rio_Branco" */
		o = -18000;
		break;
	case 0xbc5b7183: /* UTC-04:00 - "America/Halifax" */
		o = -14400;
		break;
	case 0xcbbc3b04: /* UTC-04:00 - "America/Barbados" */
		o = -14400;
		break;
	case 0x3d4bb1c4: /* UTC-04:00 - "Atlantic/Bermuda" */
		o = -14400;
		break;
	case 0x0a7b7efe: /* UTC-04:00 - "America/Boa_Vista" */
		o = -14400;
		break;
	case 0x3be064f4: /* UTC-04:00 - "America/Caracas" */
		o = -14400;
		break;
	case 0x6a879184: /* UTC-04:00 - "America/Curacao" */
		o = -14400;
		break;
	case 0x6e216197: /* UTC-04:00 - "America/Grand_Turk" */
		o = -14400;
		break;
	case 0x9ff7bd0b: /* UTC-04:00 - "America/Guyana" */
		o = -14400;
		break;
	case 0xaa29125d: /* UTC-04:00 - "America/La_Paz" */
		o = -14400;
		break;
	case 0xac86bf8b: /* UTC-04:00 - "America/Manaus" */
		o = -14400;
		break;
	case 0x551e84c5: /* UTC-04:00 - "America/Martinique" */
		o = -14400;
		break;
	case 0xd8b28d59: /* UTC-04:00 - "America/Port_of_Spain" */
		o = -14400;
		break;
	case 0x6b1aac77: /* UTC-04:00 - "America/Porto_Velho" */
		o = -14400;
		break;
	case 0x6752ca31: /* UTC-04:00 - "America/Puerto_Rico" */
		o = -14400;
		break;
	case 0x75a0d177: /* UTC-04:00 - "America/Santo_Domingo" */
		o = -14400;
		break;
	case 0x9921dd68: /* UTC-04:00 - "America/Thule" */
		o = -14400;
		break;
	case 0x04b14e6e: /* UTC-03:30 - "America/St_Johns" */
		o = -12600;
		break;
	case 0x6f9a3aef: /* UTC-03:00 - "America/Araguaina" */
		o = -10800;
		break;
	case 0x50ec79a6: /* UTC-03:00 - "America/Asuncion" */
		o = -10800;
		break;
	case 0x97da580b: /* UTC-03:00 - "America/Belem" */
		o = -10800;
		break;
	case 0xd43b4c0d: /* UTC-03:00 - "America/Argentina/Buenos_Aires" */
		o = -10800;
		break;
	case 0xfec3e7a6: /* UTC-03:00 - "America/Campo_Grande" */
		o = -10800;
		break;
	case 0x3c617269: /* UTC-03:00 - "America/Cayenne" */
		o = -10800;
		break;
	case 0x969a52eb: /* UTC-03:00 - "America/Cuiaba" */
		o = -10800;
		break;
	case 0x2ad018ee: /* UTC-03:00 - "America/Fortaleza" */
		o = -10800;
		break;
	case 0x8f7eba1f: /* UTC-03:00 - "America/Godthab" */
		o = -10800;
		break;
	case 0xac80c6d4: /* UTC-03:00 - "America/Maceio" */
		o = -10800;
		break;
	case 0x59674330: /* UTC-03:00 - "America/Miquelon" */
		o = -10800;
		break;
	case 0xfa214780: /* UTC-03:00 - "America/Montevideo" */
		o = -10800;
		break;
	case 0x40962f4f: /* UTC-03:00 - "Antarctica/Palmer" */
		o = -10800;
		break;
	case 0xb319e4c4: /* UTC-03:00 - "America/Paramaribo" */
		o = -10800;
		break;
	case 0xc2c3bce7: /* UTC-03:00 - "America/Punta_Arenas" */
		o = -10800;
		break;
	case 0xb8730494: /* UTC-03:00 - "America/Recife" */
		o = -10800;
		break;
	case 0x0e86d203: /* UTC-03:00 - "Antarctica/Rothera" */
		o = -10800;
		break;
	case 0x97d815fb: /* UTC-03:00 - "America/Bahia" */
		o = -10800;
		break;
	case 0x7410c9bc: /* UTC-03:00 - "America/Santiago" */
		o = -10800;
		break;
	case 0x7bb3e1c4: /* UTC-03:00 - "Atlantic/Stanley" */
		o = -10800;
		break;
	case 0xab5116fb: /* UTC-02:00 - "America/Noronha" */
		o = -7200;
		break;
	case 0x1063bfc9: /* UTC-02:00 - "America/Sao_Paulo" */
		o = -7200;
		break;
	case 0x33013174: /* UTC-02:00 - "Atlantic/South_Georgia" */
		o = -7200;
		break;
	case 0xf93ed918: /* UTC-01:00 - "Atlantic/Azores" */
		o = -3600;
		break;
	case 0x5c5e1772: /* UTC-01:00 - "Atlantic/Cape_Verde" */
		o = -3600;
		break;
	case 0x123f8d2a: /* UTC-01:00 - "America/Scoresbysund" */
		o = -3600;
		break;
	case 0xc21305a3: /* UTC+00:00 - "Africa/Abidjan" */
		o = 0;
		break;
	case 0x77d5b054: /* UTC+00:00 - "Africa/Accra" */
		o = 0;
		break;
	case 0x75564141: /* UTC+00:00 - "Africa/Bissau" */
		o = 0;
		break;
	case 0xfc23f2c2: /* UTC+00:00 - "Atlantic/Canary" */
		o = 0;
		break;
	case 0xc59f1b33: /* UTC+00:00 - "Africa/Casablanca" */
		o = 0;
		break;
	case 0xf554d204: /* UTC+00:00 - "America/Danmarkshavn" */
		o = 0;
		break;
	case 0x4a275f62: /* UTC+00:00 - "Europe/Dublin" */
		o = 0;
		break;
	case 0x9d6fb118: /* UTC+00:00 - "Africa/El_Aaiun" */
		o = 0;
		break;
	case 0xe110a971: /* UTC+00:00 - "Atlantic/Faroe" */
		o = 0;
		break;
	case 0xd8e31abc: /* UTC+00:00 - "Etc/UTC" */
		o = 0;
		break;
	case 0x5c00a70b: /* UTC+00:00 - "Europe/Lisbon" */
		o = 0;
		break;
	case 0x5c6a84ae: /* UTC+00:00 - "Europe/London" */
		o = 0;
		break;
	case 0x0ce90385: /* UTC+00:00 - "Africa/Monrovia" */
		o = 0;
		break;
	case 0x1c2b4f74: /* UTC+00:00 - "Atlantic/Reykjavik" */
		o = 0;
		break;
	case 0xd94515c1: /* UTC+01:00 - "Africa/Algiers" */
		o = 3600;
		break;
	case 0x109395c2: /* UTC+01:00 - "Europe/Amsterdam" */
		o = 3600;
		break;
	case 0x97f6764b: /* UTC+01:00 - "Europe/Andorra" */
		o = 3600;
		break;
	case 0x44644c20: /* UTC+01:00 - "Europe/Berlin" */
		o = 3600;
		break;
	case 0xdee07337: /* UTC+01:00 - "Europe/Brussels" */
		o = 3600;
		break;
	case 0x9ce0197c: /* UTC+01:00 - "Europe/Budapest" */
		o = 3600;
		break;
	case 0xe0532b3a: /* UTC+01:00 - "Europe/Belgrade" */
		o = 3600;
		break;
	case 0x65ee5d48: /* UTC+01:00 - "Europe/Prague" */
		o = 3600;
		break;
	case 0x77fb46ec: /* UTC+01:00 - "Africa/Ceuta" */
		o = 3600;
		break;
	case 0xe0ed30bc: /* UTC+01:00 - "Europe/Copenhagen" */
		o = 3600;
		break;
	case 0xf8e325fc: /* UTC+01:00 - "Europe/Gibraltar" */
		o = 3600;
		break;
	case 0x789bb5d0: /* UTC+01:00 - "Africa/Lagos" */
		o = 3600;
		break;
	case 0x1f8bc6ce: /* UTC+01:00 - "Europe/Luxembourg" */
		o = 3600;
		break;
	case 0x5dbd1535: /* UTC+01:00 - "Europe/Madrid" */
		o = 3600;
		break;
	case 0xfb1560f3: /* UTC+01:00 - "Europe/Malta" */
		o = 3600;
		break;
	case 0x5ebf9f01: /* UTC+01:00 - "Europe/Monaco" */
		o = 3600;
		break;
	case 0x9fe09898: /* UTC+01:00 - "Africa/Ndjamena" */
		o = 3600;
		break;
	case 0xa2c3fba1: /* UTC+01:00 - "Europe/Oslo" */
		o = 3600;
		break;
	case 0xfb4bc2a3: /* UTC+01:00 - "Europe/Paris" */
		o = 3600;
		break;
	case 0xa2c58fd7: /* UTC+01:00 - "Europe/Rome" */
		o = 3600;
		break;
	case 0x5bf6fbb8: /* UTC+01:00 - "Europe/Stockholm" */
		o = 3600;
		break;
	case 0x6ea95b47: /* UTC+01:00 - "Europe/Tirane" */
		o = 3600;
		break;
	case 0x79378e6d: /* UTC+01:00 - "Africa/Tunis" */
		o = 3600;
		break;
	case 0x734cc2e5: /* UTC+01:00 - "Europe/Vienna" */
		o = 3600;
		break;
	case 0x75185c19: /* UTC+01:00 - "Europe/Warsaw" */
		o = 3600;
		break;
	case 0x7d8195b9: /* UTC+01:00 - "Europe/Zurich" */
		o = 3600;
		break;
	case 0x148d21bc: /* UTC+02:00 - "Asia/Amman" */
		o = 7200;
		break;
	case 0x4318fa27: /* UTC+02:00 - "Europe/Athens" */
		o = 7200;
		break;
	case 0xa7f3d5fd: /* UTC+02:00 - "Asia/Beirut" */
		o = 7200;
		break;
	case 0xfb349ec5: /* UTC+02:00 - "Europe/Bucharest" */
		o = 7200;
		break;
	case 0x77f8e228: /* UTC+02:00 - "Africa/Cairo" */
		o = 7200;
		break;
	case 0xad58aa18: /* UTC+02:00 - "Europe/Chisinau" */
		o = 7200;
		break;
	case 0x20fbb063: /* UTC+02:00 - "Asia/Damascus" */
		o = 7200;
		break;
	case 0x1faa4875: /* UTC+02:00 - "Asia/Gaza" */
		o = 7200;
		break;
	case 0x6ab2975b: /* UTC+02:00 - "Europe/Helsinki" */
		o = 7200;
		break;
	case 0x5becd23a: /* UTC+02:00 - "Asia/Jerusalem" */
		o = 7200;
		break;
	case 0xd5d157a0: /* UTC+02:00 - "Africa/Johannesburg" */
		o = 7200;
		break;
	case 0xfb3d4205: /* UTC+02:00 - "Africa/Khartoum" */
		o = 7200;
		break;
	case 0xa2c19eb3: /* UTC+02:00 - "Europe/Kiev" */
		o = 7200;
		break;
	case 0x8e6ca1f0: /* UTC+02:00 - "Africa/Maputo" */
		o = 7200;
		break;
	case 0xd33b2f28: /* UTC+02:00 - "Europe/Kaliningrad" */
		o = 7200;
		break;
	case 0x4b0fcf78: /* UTC+02:00 - "Asia/Nicosia" */
		o = 7200;
		break;
	case 0xa2c57587: /* UTC+02:00 - "Europe/Riga" */
		o = 7200;
		break;
	case 0xfb898656: /* UTC+02:00 - "Europe/Sofia" */
		o = 7200;
		break;
	case 0x30c4e096: /* UTC+02:00 - "Europe/Tallinn" */
		o = 7200;
		break;
	case 0x9dfebd3d: /* UTC+02:00 - "Africa/Tripoli" */
		o = 7200;
		break;
	case 0xdd63b8ce: /* UTC+02:00 - "Europe/Vilnius" */
		o = 7200;
		break;
	case 0x789c9bd3: /* UTC+02:00 - "Africa/Windhoek" */
		o = 7200;
		break;
	case 0x9ceffbed: /* UTC+03:00 - "Asia/Baghdad" */
		o = 10800;
		break;
	case 0x9e09d6e6: /* UTC+03:00 - "Europe/Istanbul" */
		o = 10800;
		break;
	case 0xfb19cc66: /* UTC+03:00 - "Europe/Minsk" */
		o = 10800;
		break;
	case 0x5ec266fc: /* UTC+03:00 - "Europe/Moscow" */
		o = 10800;
		break;
	case 0xa87ab57e: /* UTC+03:00 - "Africa/Nairobi" */
		o = 10800;
		break;
	case 0x15a8330b: /* UTC+03:00 - "Asia/Qatar" */
		o = 10800;
		break;
	case 0xcd973d93: /* UTC+03:00 - "Asia/Riyadh" */
		o = 10800;
		break;
	case 0xe330c7e1: /* UTC+03:00 - "Antarctica/Syowa" */
		o = 10800;
		break;
	case 0xd1f02254: /* UTC+03:30 - "Asia/Tehran" */
		o = 12600;
		break;
	case 0x1fa788b5: /* UTC+04:00 - "Asia/Baku" */
		o = 14400;
		break;
	case 0x14c79f77: /* UTC+04:00 - "Asia/Dubai" */
		o = 14400;
		break;
	case 0x45e725e2: /* UTC+04:00 - "Indian/Mahe" */
		o = 14400;
		break;
	case 0x7b09c02a: /* UTC+04:00 - "Indian/Mauritius" */
		o = 14400;
		break;
	case 0x6bc0b139: /* UTC+04:00 - "Europe/Samara" */
		o = 14400;
		break;
	case 0x7076c047: /* UTC+04:00 - "Indian/Reunion" */
		o = 14400;
		break;
	case 0x0903e442: /* UTC+04:00 - "Asia/Tbilisi" */
		o = 14400;
		break;
	case 0x9185c8cc: /* UTC+04:00 - "Asia/Yerevan" */
		o = 14400;
		break;
	case 0x153b5601: /* UTC+04:30 - "Asia/Kabul" */
		o = 16200;
		break;
	case 0x148f710e: /* UTC+05:00 - "Asia/Aqtau" */
		o = 18000;
		break;
	case 0xa67dcc4e: /* UTC+05:00 - "Asia/Aqtobe" */
		o = 18000;
		break;
	case 0xba87598d: /* UTC+05:00 - "Asia/Ashgabat" */
		o = 18000;
		break;
	case 0x32fc5c3c: /* UTC+05:00 - "Asia/Dushanbe" */
		o = 18000;
		break;
	case 0x527f5245: /* UTC+05:00 - "Asia/Karachi" */
		o = 18000;
		break;
	case 0x4351b389: /* UTC+05:00 - "Indian/Kerguelen" */
		o = 18000;
		break;
	case 0x9869681c: /* UTC+05:00 - "Indian/Maldives" */
		o = 18000;
		break;
	case 0x399cd863: /* UTC+05:00 - "Antarctica/Mawson" */
		o = 18000;
		break;
	case 0xfb544c6e: /* UTC+05:00 - "Asia/Yekaterinburg" */
		o = 18000;
		break;
	case 0xf3924254: /* UTC+05:00 - "Asia/Tashkent" */
		o = 18000;
		break;
	case 0x0af0e91d: /* UTC+05:30 - "Asia/Colombo" */
		o = 19800;
		break;
	case 0x328a44c3: /* UTC+05:30 - "Asia/Calcutta" */
		o = 19800;
		break;
	case 0x9a96ce6f: /* UTC+05:45 - "Asia/Kathmandu" */
		o = 20700;
		break;
	case 0xa61f41fa: /* UTC+06:00 - "Asia/Almaty" */
		o = 21600;
		break;
	case 0xb0728553: /* UTC+06:00 - "Asia/Bishkek" */
		o = 21600;
		break;
	case 0x456f7c3c: /* UTC+06:00 - "Indian/Chagos" */
		o = 21600;
		break;
	case 0x14c07b8b: /* UTC+06:00 - "Asia/Dhaka" */
		o = 21600;
		break;
	case 0x1faeddac: /* UTC+06:00 - "Asia/Omsk" */
		o = 21600;
		break;
	case 0x170380d1: /* UTC+06:00 - "Asia/Thimphu" */
		o = 21600;
		break;
	case 0x4f966fd4: /* UTC+06:00 - "Antarctica/Vostok" */
		o = 21600;
		break;
	case 0x021e86de: /* UTC+06:30 - "Indian/Cocos" */
		o = 23400;
		break;
	case 0xdd54a8be: /* UTC+06:30 - "Asia/Yangon" */
		o = 23400;
		break;
	case 0x9d6e3aaf: /* UTC+07:00 - "Asia/Bangkok" */
		o = 25200;
		break;
	case 0x68c207d5: /* UTC+07:00 - "Indian/Christmas" */
		o = 25200;
		break;
	case 0xe2144b45: /* UTC+07:00 - "Antarctica/Davis" */
		o = 25200;
		break;
	case 0xcf52f713: /* UTC+07:00 - "Asia/Saigon" */
		o = 25200;
		break;
	case 0x1fab0fe3: /* UTC+07:00 - "Asia/Hovd" */
		o = 25200;
		break;
	case 0x0506ab50: /* UTC+07:00 - "Asia/Jakarta" */
		o = 25200;
		break;
	case 0xd0376c6a: /* UTC+07:00 - "Asia/Krasnoyarsk" */
		o = 25200;
		break;
	case 0xa8e595f7: /* UTC+08:00 - "Asia/Brunei" */
		o = 28800;
		break;
	case 0xf895a7f5: /* UTC+08:00 - "Asia/Shanghai" */
		o = 28800;
		break;
	case 0x928aa4a6: /* UTC+08:00 - "Asia/Choibalsan" */
		o = 28800;
		break;
	case 0x577f28ac: /* UTC+08:00 - "Asia/Hong_Kong" */
		o = 28800;
		break;
	case 0x014763c4: /* UTC+08:00 - "Asia/Kuala_Lumpur" */
		o = 28800;
		break;
	case 0x155f88b9: /* UTC+08:00 - "Asia/Macau" */
		o = 28800;
		break;
	case 0x6aa21c85: /* UTC+08:00 - "Asia/Makassar" */
		o = 28800;
		break;
	case 0xc156c944: /* UTC+08:00 - "Asia/Manila" */
		o = 28800;
		break;
	case 0xdfbf213f: /* UTC+08:00 - "Asia/Irkutsk" */
		o = 28800;
		break;
	case 0xcf8581fa: /* UTC+08:00 - "Asia/Singapore" */
		o = 28800;
		break;
	case 0xd1a844ae: /* UTC+08:00 - "Asia/Taipei" */
		o = 28800;
		break;
	case 0x30f0cc4e: /* UTC+08:00 - "Asia/Ulaanbaatar" */
		o = 28800;
		break;
	case 0x8db8269d: /* UTC+08:00 - "Australia/Perth" */
		o = 28800;
		break;
	case 0x93ed1c8e: /* UTC+08:30 - "Asia/Pyongyang" */
		o = 30600;
		break;
	case 0x1fa8c394: /* UTC+09:00 - "Asia/Dili" */
		o = 32400;
		break;
	case 0xc6833c2f: /* UTC+09:00 - "Asia/Jayapura" */
		o = 32400;
		break;
	case 0x87bb3a9e: /* UTC+09:00 - "Asia/Yakutsk" */
		o = 32400;
		break;
	case 0x8af04a36: /* UTC+09:00 - "Pacific/Palau" */
		o = 32400;
		break;
	case 0x15ce82da: /* UTC+09:00 - "Asia/Seoul" */
		o = 32400;
		break;
	case 0x15e606a8: /* UTC+09:00 - "Asia/Tokyo" */
		o = 32400;
		break;
	case 0x2876bdff: /* UTC+09:30 - "Australia/Darwin" */
		o = 34200;
		break;
	case 0x5a3c656c: /* UTC+10:00 - "Antarctica/DumontDUrville" */
		o = 36000;
		break;
	case 0x4fedc9c0: /* UTC+10:00 - "Australia/Brisbane" */
		o = 36000;
		break;
	case 0x2338f9ed: /* UTC+10:00 - "Pacific/Guam" */
		o = 36000;
		break;
	case 0x29de34a8: /* UTC+10:00 - "Asia/Vladivostok" */
		o = 36000;
		break;
	case 0xa7ba7f68: /* UTC+10:00 - "Pacific/Port_Moresby" */
		o = 36000;
		break;
	case 0x8a090b23: /* UTC+10:00 - "Pacific/Chuuk" */
		o = 36000;
		break;
	case 0x2428e8a3: /* UTC+10:30 - "Australia/Adelaide" */
		o = 37800;
		break;
	case 0xe2022583: /* UTC+11:00 - "Antarctica/Casey" */
		o = 39600;
		break;
	case 0x32bf951a: /* UTC+11:00 - "Australia/Hobart" */
		o = 39600;
		break;
	case 0x4d1e9776: /* UTC+11:00 - "Australia/Sydney" */
		o = 39600;
		break;
	case 0x8a2bce28: /* UTC+11:00 - "Pacific/Efate" */
		o = 39600;
		break;
	case 0xf4dd25f0: /* UTC+11:00 - "Pacific/Guadalcanal" */
		o = 39600;
		break;
	case 0xde5139a8: /* UTC+11:00 - "Pacific/Kosrae" */
		o = 39600;
		break;
	case 0xebacc19b: /* UTC+11:00 - "Asia/Magadan" */
		o = 39600;
		break;
	case 0x8f4eb4be: /* UTC+11:00 - "Pacific/Norfolk" */
		o = 39600;
		break;
	case 0xe551b788: /* UTC+11:00 - "Pacific/Noumea" */
		o = 39600;
		break;
	case 0x28929f96: /* UTC+11:00 - "Pacific/Pohnpei" */
		o = 39600;
		break;
	case 0xdb402d65: /* UTC+12:00 - "Pacific/Funafuti" */
		o = 43200;
		break;
	case 0x8e216759: /* UTC+12:00 - "Pacific/Kwajalein" */
		o = 43200;
		break;
	case 0xe1f95371: /* UTC+12:00 - "Pacific/Majuro" */
		o = 43200;
		break;
	case 0x73baf9d7: /* UTC+12:00 - "Asia/Kamchatka" */
		o = 43200;
		break;
	case 0x8acc41ae: /* UTC+12:00 - "Pacific/Nauru" */
		o = 43200;
		break;
	case 0xf2517e63: /* UTC+12:00 - "Pacific/Tarawa" */
		o = 43200;
		break;
	case 0x23416c2b: /* UTC+12:00 - "Pacific/Wake" */
		o = 43200;
		break;
	case 0xf94ddb0f: /* UTC+12:00 - "Pacific/Wallis" */
		o = 43200;
		break;
	case 0x25062f86: /* UTC+13:00 - "Pacific/Auckland" */
		o = 46800;
		break;
	case 0x61599a93: /* UTC+13:00 - "Pacific/Enderbury" */
		o = 46800;
		break;
	case 0x06532bba: /* UTC+13:00 - "Pacific/Fakaofo" */
		o = 46800;
		break;
	case 0x23383ba5: /* UTC+13:00 - "Pacific/Fiji" */
		o = 46800;
		break;
	case 0x262ca836: /* UTC+13:00 - "Pacific/Tongatapu" */
		o = 46800;
		break;
	case 0x23359b5e: /* UTC+14:00 - "Pacific/Apia" */
		o = 50400;
		break;
	case 0x8305073a: /* UTC+14:00 - "Pacific/Kiritimati" */
		o = 50400;
		break;
	default:
		break;
	}
	return o;
}
EXPORT_SYMBOL(timezone);
