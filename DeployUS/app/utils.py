"""
DeployUS/app/utils.py
"""
from hashlib import sha256
from datetime import datetime, timedelta, timezone

# Wanted DATETIME format for the project. Omit the seconds
DATETIME_FORMAT = "%Y-%m-%d %H:%M"


def get_hash(bytestream: bytes):
    """
    Returns the hex representation of the SHA256 hash of the bytestream.

    Parameters:
      bytestream. ReadableBuffer. Array of bytes to hash.
    Returns:
      SHA256 hash.
    """
    hashobj = sha256()
    hashobj.update(bytestream)
    return hashobj.hexdigest()


def get_datetime_now():
    """
    Finds the current time. Assumes UTC -4:00 timezone.
    Returns:
      Formated datetime in UTC -4:00 timezone. Format is "%Y-%m-%d %H:%M"
    """
    tzinfo = timezone(timedelta(hours=-4))  # TimeZone UTC -4:00
    return datetime.now(tzinfo).strftime(DATETIME_FORMAT)


def format_datetime_obj(date: str):
    """
    Convert string in "%Y-%m-%d %H:%M" format into python datetime object
    Parameters:
      date: str. String representation of datetime obj in "%Y-%m-%d %H:%M" format.
    Returns:
      Valid python datetime object.
    """
    year = int(date.split(" ")[0].split("-")[0])
    month = int(date.split(" ")[0].split("-")[1])
    day = int(date.split(" ")[0].split("-")[2])
    hour = int(date.split(" ")[1].split(":")[0])
    minute = int(date.split(" ")[1].split(":")[1])
    seconds = int(date.split(" ")[1].split(":")[2])

    return datetime(year, month, day, hour, minute, seconds).strftime(DATETIME_FORMAT)
