bool sendToSXKL(int what)
{
	IRA::CSocket sock;
	IRA::CError err;
	IRA::CString addr(SXKL_ADDRESS);
	char readout[128];
	int buf[3];
	int count;
	DWORD port=SXKL_PORT;
	IRA::CString buffer;
	try {
		if (sock.Create(err,IRA::CSocket::STREAM)!=IRA::CSocket::SUCCESS) {
			return false;
		}
		if (sock.Connect(err,addr,port)==IRA::CSocket::FAIL) {
			return false;
		}
		// declare myself to server		
		buffer="structuralclient0001";
		if (sock.Send(err,buffer,buffer.GetLength())!=buffer.GetLength()) {
			return false;
		}
		count=sock.Receive(err,(void *)readout,128);
		if (count==IRA::CSocket::FAIL) {
			return false;
		}
		readout[count]=0;
		// read current status 
  		buffer="get(";
		if (sock.Send(err,buffer,4)!=4) {
			return false;
		}
		count=sock.Receive(err,(void *)readout,128);
		if (count==IRA::CSocket::FAIL) {
			return false;
		}
		readout[count]=0;
		sscanf(readout,"OK%02X%04X%02X",buf,buf+1,buf+2);
		if (what==1) { //calon
			buffer.Format("send( %02X0000%02X",buf[0],buf[2] | 0x40);
		}
		else if (what==2) { //caloff
			buffer.Format("send( %02X0000%02X",buf[0],buf[2] & ~0x40);
		}
		else { //setup
			buffer="send%0068033f";
		}
		if (sock.Send(err,buffer,buffer.GetLength())!=buffer.GetLength()) {
			return false;
		}
		count=sock.Receive(err,(void *)readout,128);
		if (count==IRA::CSocket::FAIL) {
			return false;
		}
		readout[count]=0;		
		sock.Close(err);
	}
	catch (...) {
		return false;		
	}
	return true;
}