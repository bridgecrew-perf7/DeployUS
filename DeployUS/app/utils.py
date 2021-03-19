import sys

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