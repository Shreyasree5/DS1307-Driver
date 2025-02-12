import smbus
import time

# I2C setup
I2C_BUS = 1  # Use I2C bus 1 on Raspberry Pi
DS1307_ADDRESS = 0x68  # DS1307 I2C address

# Create an SMBus instance
bus = smbus.SMBus(I2C_BUS)
start_register=0x00

def validate(date_time):
    if not (0 <= date_time["seconds"] <= 59):
        return False, "Seconds must be in range 0-59"
    if not (0 <= date_time["minutes"] <= 59):
        return False, "Minutes must be in range 0-59"
    if ('AMPM' in date_time.keys()):
        if (date_time["AMPM"]=="AM" or date_time["AMPM"]=="PM"):
            if not(date_time["hours"]>=0 and date_time["hours"]<=12):
                return False, "Hours must be in range 0-12"  
        if not (1 <= date_time["day"] <= 7):
            return False, "Day must be in range 1-7 (Monday-Sunday)"
    if not (1 <= date_time["date"] <= 31):
        return False, "Date must be in range 1-31"
    if not (1 <= date_time["month"] <= 12):
        return False, "Month must be in range 1-12"
    if not (0 <= date_time["year"] <= 99):
        return False, "Year must be in range 0-99 (last two digits)"
    return True, "Validation successful"

def decimal_to_bcd(n): #bcd = binary coded decimal
        a= n//10
        b= n%10
        return (a<<4)|b

def bcd_to_decimal(n):
       a=n>>4
       b= n&0x0F
       return (a*10)+b

def setTime(date_time):
        is_valid, message = validate(date_time_to_set)
        if is_valid:
            print(message)
            n = decimal_to_bcd(date_time["hours"])
            
            if ('AMPM' in date_time.keys()):
                n = n | (1<<6)
                if (date_time["AMPM"]=='PM'):
                    n = (1<<5)|n
                else:
                    n = ~(1<<5)&n
            else:
                n = n & ~(1<<6)

            date_time_bcd = [decimal_to_bcd(date_time["seconds"]), decimal_to_bcd(date_time["minutes"]), n, decimal_to_bcd(date_time["day"]), decimal_to_bcd(date_time["date"]), decimal_to_bcd(date_time["month"]), decimal_to_bcd(date_time["year"])]
            bus.write_i2c_block_data(DS1307_ADDRESS, start_register, date_time_bcd)
            print("Date and Time set succussfully:", date_time)
            return 0
        else:
            print(f"Validation failed: {message}")
            return -1        

def getTime():
    data_time = bus.read_i2c_block_data(DS1307_ADDRESS, start_register, 7)
    n =((data_time[2] & (1<<6))>>6)
    if ( n != 0):
        if ((1<<5)& data_time[2] != 0):
            ampm = 'PM'
        else:
            ampm = 'AM'

        ret = {
            "seconds": bcd_to_decimal(data_time[0]),
            "minutes":bcd_to_decimal(data_time[1]),
            "hours":bcd_to_decimal(data_time[2]&0x1F),
            "day":bcd_to_decimal(data_time[3]),
            "date":bcd_to_decimal(data_time[4]),
            "month":bcd_to_decimal(data_time[5]),
            "year":bcd_to_decimal(data_time[6]),
            "AMPM":ampm
        }
        return ret
    else:
        return {
            "seconds": bcd_to_decimal(data_time[0]),
            "minutes":bcd_to_decimal(data_time[1]),
            "hours":bcd_to_decimal(data_time[2]&0x1F),
            "day":bcd_to_decimal(data_time[3]),
            "date":bcd_to_decimal(data_time[4]),
            "month":bcd_to_decimal(data_time[5]),
            "year":bcd_to_decimal(data_time[6]),
        }

#input date and time to ds1307
date_time_to_set = {
       "seconds":45,
       "minutes":22,
       "hours":7,
       "day":7,
       "date":5,
       "month":9,
       "year":99,
       "AMPM": "AM"
}

setTime(date_time_to_set)

#print the date and time in output
current_time = getTime()
print("Current Time: ", current_time)


       