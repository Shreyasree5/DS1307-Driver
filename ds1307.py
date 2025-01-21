import smbus

# I2C setup
I2C_BUS = 1  # Use I2C bus 1 on Raspberry Pi
DS1307_ADDRESS = 0x68  # DS1307 I2C address

# Create an SMBus instance
bus = smbus.SMBus(I2C_BUS)

def dec_to_bcd(n):
	return (n // 10) << 4 | (n % 10)

def bcd_to_dec(n):
	return (n >> 4) * 10 + (n & 0x0F)

def write_register(register, data):
	bus.write_i2c_block_data(DS1307_ADDRESS, register, data)

def read_register(register, length):
    return bus.read_i2c_block_data(DS1307_ADDRESS, register, length)

date_time = {
	"seconds": 45,
	"minutes": 22,
	"hours": 14,
	"day": 7,
	"date": 5,
	"month": 9,
	"year": 99 
}

def validate(date_time):
    if not (0 <= date_time["seconds"] <= 59):
        return False, "Seconds must be in range 0-59"
    if not (0 <= date_time["minutes"] <= 59):
        return False, "Minutes must be in range 0-59"
    if not (0 <= date_time["hours"] <= 23):
        return False, "Hours must be in range 0-23"
    if not (1 <= date_time["day"] <= 7):
        return False, "Day must be in range 1-7 (Monday-Sunday)"
    if not (1 <= date_time["date"] <= 31):
        return False, "Date must be in range 1-31"
    if not (1 <= date_time["month"] <= 12):
        return False, "Month must be in range 1-12"
    if not (0 <= date_time["year"] <= 99):
        return False, "Year must be in range 0-99 (last two digits)"
    return True, "Validation successful"

is_valid, message = validate(date_time)

if is_valid:
	print(message)
	date_time_bcd = {key: dec_to_bcd(value) for key, value in date_time.items()}
	write_data= [date_time_bcd["seconds"], date_time_bcd["minutes"], date_time_bcd["hours"], date_time_bcd["day"], date_time_bcd["date"], date_time_bcd["month"], date_time_bcd["year"]]
 
	write_register(0x00, write_data)
	print(f"Date & Time set: {date_time}")
else:
	print(f"validation failed:{message}") 

raw_data = read_register(0x00, 7)

seconds = bcd_to_dec(raw_data[0] & 0x7F)  # Mask out CH bit
minutes = bcd_to_dec(raw_data[1])
hours = bcd_to_dec(raw_data[2] & 0x3F)   # Mask for 24-hour format
day = bcd_to_dec(raw_data[3])
date = bcd_to_dec(raw_data[4])
month = bcd_to_dec(raw_data[5])
year = bcd_to_dec(raw_data[6])

# Print the decoded Date & Time
print(f"Read Date & Time: {date:02}-{month:02}-{year:02} {hours:02}:{minutes:02}:{seconds:02}")
  

