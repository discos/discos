#!/usr/bin/env python
from google.oauth2.credentials import Credentials
from google.auth.transport.requests import Request
from googleapiclient.discovery import build
from googleapiclient.http import MediaIoBaseDownload
import os
import io

TOKEN_FILE = 'token.json'
VM_FILE_PATH = '/home/runner/discos_manager.ova'
ARCHIVE_FILE_PATH = '/home/runner/vagrant.tar.gz'
SCOPES = ['https://www.googleapis.com/auth/drive.readonly']

# Create the token file from the GH Secret
with open(TOKEN_FILE, 'w') as tokenfile:
    tokenfile.write(os.environ.get('GOOGLE_DRIVE_TOKEN'))

# Authenticate with the token and eventually update it
creds = Credentials.from_authorized_user_file(TOKEN_FILE, SCOPES)
if creds.expired and creds.refresh_token:
    creds.refresh(Request())

# Download the VM
creds = Credentials.from_authorized_user_file(TOKEN_FILE, SCOPES)
if creds.expired and creds.refresh_token:
    creds.refresh(Request())

service = build('drive', 'v3', credentials=creds)

downloader = MediaIoBaseDownload(
    io.FileIO(ARCHIVE_FILE_PATH, 'wb'),
    service.files().get_media(
        fileId=os.environ.get('PROVISIONED_ARCHIVE_GDRIVE_ID')
    )
)
done = False
while not done:
    _, done = downloader.next_chunk()

downloader = MediaIoBaseDownload(
    io.FileIO(VM_FILE_PATH, 'wb'),
    service.files().get_media(
        fileId=os.environ.get('PROVISIONED_VM_GDRIVE_ID'),
    ),
    chunksize=5*1024*1024
)
done = False
while not done:
    _, done = downloader.next_chunk()

# Finally update the token file
with open(TOKEN_FILE, 'w') as tokenfile:
    tokenfile.write(creds.to_json())
