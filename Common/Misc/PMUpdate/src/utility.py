
def convert_time_doy_to_string_format(date_time_doy_string):
    # function to convert datetime in format ex. [00006,  2018,  094,  18,  12,  28, 0]
    # in UTC time utc_time = datetime.datetime.strptime(row_value[0], "%Y-%b-%d %H:%M:%S").replace(tzinfo=pytz.UTC)

    doy_year, doy_day = date_time_doy_string[1:3]
    hours, minutes, seconds = date_time_doy_string[3:6]
    year, month, day = DATE(doy_day, doy_year)

    return "{year}-{month}-{day} {hours}:{minutes}:{seconds}".format(year=year, month=month, day=day, hours=hours, minutes=minutes, seconds=seconds)



def DATE(DAY, YEAR):
    DAY = int(DAY)
    YEAR = float(YEAR)

    # Y=1
    # Y=0

    if (YEAR//400 == YEAR/400):
        Y=1

    elif (YEAR//4 == YEAR/4) and (YEAR//100 != YEAR/100):
        Y=1

    else:
        Y=0


    MONTH_LEN   = [31,28,31,30,31,30,31,31,30,31,30,31]


    if Y==0:
        MONTH_LEN=MONTH_LEN

    else:
        MONTH_LEN[1]+=1


    if Y == 0 and (DAY > 365 or DAY < 1):
        raise RuntimeError, str(int(YEAR))+" Number of day less then 1 or more then 365"

    if Y == 1 and (DAY > 366 or DAY < 1):
        raise RuntimeError, str(int(YEAR))+" Number of day less then 1 or more then 366"

    for N, M in enumerate(MONTH_LEN):
        if DAY > M:
            DAY = DAY-M
        else:
            M_NUM = N+1
            break

    if len(str(DAY)) == 1:
        DAY = '0' + str(DAY)
    else:
        DAY = str(DAY)

    if len(str(M_NUM)) == 1:
        M_NUM = '0' + str(M_NUM)
    else:
        M_NUM = str(M_NUM)

    return int(YEAR), int(M_NUM), int(DAY)