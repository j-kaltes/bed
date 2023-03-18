	char months[][4]= { "Jan","Feb","Mar","Apr","May", "Jun", "Jul", "Aug","Sep","Oct","Nov" ,"Dec" };
	const char *ptr=formal+4;
	int timtm_year, timtm_hour;
	for(int j=0;j<12;j++) {
		if(!strncasecmp(months[j],	ptr,3)) {
				tim.tm_mon=j;
				tim.tm_isdst=0;
				tim.tm_wday=0;
				ptr+=4;
sscanf(ptr,"%d %d:%d:%d %d", &tim.tm_mday,&timtm_hour,&tim.tm_min,&tim.tm_sec,&timtm_year);
/*				if(timtm_hour>24||timtm_hour<0||tim.tm_min<0||tim.tm_min>60||tim.tm_mday<0||tim.tm_mday>32||timtm_year<1901||timtm_year>2037)
					return -1;
*/
				timtm_year-=1900;
				tim.tm_year=timtm_year;
				tim.tm_hour=timtm_hour;

				t=amktime(&tim);
	//			t+=tim.tm_gmtoff;
//	t-=timezone;
				memcpy(bin,(char *)&t,4);
				return 0;
				}

		}
	return -1;
