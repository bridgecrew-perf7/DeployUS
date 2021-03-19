import sys
from hashlib import sha256
from datetime import datetime, timedelta, timezone

DATETIME_FORMAT = "%Y-%m-%d %H:%M"

def printus(thingToPrint):
    # Function to view pints with docker logs
    print(thingToPrint, file=sys.stdout)

def allowed_file(filename):
    # Verify that files ends with .yml or .YML
    # Must have at least 1 . in filename
    if not "." in filename:
        return False

    # Check if the extension is in ALLOWED_IMAGE_EXTENSIONS
    ext = filename.rsplit(".", 1)[1]
    if ext.upper() == 'YML':
        return True
    else:
        return False

def getHash(bytes):
        hashobj = sha256()
        hashobj.update(bytes)
        return hashobj.hexdigest()

def getDatetimeNow():
    # Adding current datetime of upload
    tzinfo = timezone(timedelta(hours=-4)) # TimeZone UTC -4:00
    return datetime.now(tzinfo).strftime(DATETIME_FORMAT)

def formatDateTimeObj(date):
    # Database specific formating
    year = int(date.split(' ')[0].split('-')[0])
    month = int(date.split(' ')[0].split('-')[1])
    day = int(date.split(' ')[0].split('-')[2])
    hour = int(date.split(' ')[1].split(':')[0])
    minute = int(date.split(' ')[1].split(':')[1])
    seconds = int(date.split(' ')[1].split(':')[2])
    
    return datetime(year, month, day, hour, minute,seconds).strftime(DATETIME_FORMAT)