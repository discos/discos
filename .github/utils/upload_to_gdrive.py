#!/usr/bin/env python
from google.oauth2.credentials import Credentials
from google.auth.transport.requests import Request
from googleapiclient.discovery import build
from googleapiclient.http import MediaFileUpload
import os

TOKEN_FILE = 'token.json'
VM_FILE_PATH = '/home/runner/discos_manager.ova'
SCOPES = [
    'https://www.googleapis.com/auth/drive',
    'https://www.googleapis.com/auth/drive.file'
]

# Authenticate with the token and eventually update it
creds = Credentials.from_authorized_user_file(TOKEN_FILE, SCOPES)
if creds.expired and creds.refresh_token:
    creds.refresh(Request())

# Prepare the files to be uploaded
service = build('drive', 'v3', credentials=creds)
vm_media = MediaFileUpload(VM_FILE_PATH, resumable=True)
service.files().update(
    fileId=os.environ.get('DEPLOYED_VM_GDRIVE_ID'),
    media_body=vm_media,
    fields='id'
).execute()

# Finally update the token file
with open(TOKEN_FILE, 'w') as tokenfile:
    tokenfile.write(creds.to_json())
