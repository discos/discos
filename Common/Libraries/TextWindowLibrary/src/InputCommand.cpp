// $Id: InputCommand.cpp,v 1.4 2009-05-06 14:06:53 a.orlati Exp $

#include "TW_InputCommand.h"

#ifdef KEY_BACKSPACE
#undef KEY_BACKSPACE
#endif
#define KEY_BACKSPACE 7
#ifdef KEY_ENTER
#undef KEY_ENTER
#endif
#define KEY_ENTER 10

#ifdef KEY_UPPERARROW
#undef KEY_UPPERARROW
#endif
#define KEY_UPPERARROW 3

#ifdef KEY_LOWERARROW
#undef KEY_LOWERARROW
#endif
#define KEY_LOWERARROW 2

#ifdef KEY_LEFTARROW
#undef KEY_LEFTARROW
#endif
#define KEY_LEFTARROW 4

#ifdef KEY_RIGHTARROW
#undef KEY_RIGHTARROW
#endif
#define KEY_RIGHTARROW 5

#define MAX_COMMAND_NUMBER 10

using namespace TW;

CInputCommand::CInputCommand() : CFrameComponent(), m_textLength(256), m_delimiter(' '),
   m_inputStack(NULL),m_stackPointer(MAX_COMMAND_NUMBER),m_stackScroller(MAX_COMMAND_NUMBER),
   m_buffer(NULL),m_bufferCounter(0),m_bufferScroller(0)
{
	m_buffer=new char [m_textLength+1]; //one extra position for the string terminator
	assert(m_buffer);
	bzero(m_buffer,m_textLength+1);
	m_inputStack=new char *[MAX_COMMAND_NUMBER];
	assert(m_inputStack);
	for (int i=0;i<MAX_COMMAND_NUMBER;i++) {
		m_inputStack[i]=new char[m_textLength+1];
		assert(m_inputStack[i]);
	}
}

CInputCommand::~CInputCommand()
{
	if (m_inputStack!=NULL) {
		for (int i=0;i<MAX_COMMAND_NUMBER;i++) {
			if (m_inputStack[i]!=NULL) {
				delete []m_inputStack[i];
			}
		}
		delete [] m_inputStack;
		m_inputStack=NULL;
	}
	if (m_buffer!=NULL) {
		delete []m_buffer;
		m_buffer=NULL;
	}
}

void CInputCommand::setTextLength(const WORD& len) 
{ 
	char *tmp;
	for (int i=0;i<MAX_COMMAND_NUMBER;i++) {
		tmp=new char[len+1];
		memcpy(tmp,m_inputStack[i],MIN(len,m_textLength)+1);
		delete []m_inputStack[i];
		m_inputStack[i]=tmp;
	}
	tmp=new char[len+1];
	memcpy(tmp,m_buffer,MIN(len,m_textLength)+1);
	delete []m_buffer;
	m_buffer=tmp;
	m_textLength=len;
}

WORD CInputCommand::draw()
{
	CStyle style,cursorStyle;
	WORD width,height,chars,len;
	WORD newScroller;
	WORD curX,curY;
	CPoint newPosition;
	IRA::CString tmp,line;
	WORD step;
	if (!isAlive()) return 4;
	if (!getEnabled()) return 1;
	if (!getMainFrame()->insideBorders(getPosition())) return 2;
	height=getEffectiveHeight();
	width=getEffectiveWidth()-1;
	chars=height*width;
	len=getValue().GetLength();
	curY=(m_bufferScroller/width);
	if (len<chars) {
		tmp=getValue();
		newScroller=m_bufferScroller;
	}
	else if (m_bufferScroller<len-chars) {
		tmp=getValue().Mid(curY*width,chars);
		newScroller=m_bufferScroller-(curY*width);
	}
	else  {
		tmp=getValue().Right(chars);
		newScroller=m_bufferScroller-(len-chars);
	}
	// now tmp is my string......
	len=tmp.GetLength();
	curY=newScroller/width;
	curX=newScroller%width;
	step=len/width;
	style=getStyle();
	cursorStyle=getStyle();
	cursorStyle.addAttribute(CStyle::UNDERLINE);
	cursorStyle.addAttribute(CStyle::BLINK);
	for (int i=0;i<height;i++) {
		if (!getMainFrame()->clearCanvas(getPosition()+CPoint(0,i),width+1,style)) return 3;
		if (!getMainFrame()->writeCanvas(getPosition()+CPoint(0,i),style,">")) return 3;
		if (i<=step) {
			line=tmp.Mid(i*width,width);
			if (!getMainFrame()->writeCanvas(getPosition()+CPoint(1,i),style,line)) return 3;
			if (curY==i) {
				if (curX!=line.GetLength()) {
					line=tmp.Mid(curY*width+curX,1);
					if (!getMainFrame()->writeCanvas(getPosition()+CPoint(curX+1,i),cursorStyle,line)) return 3;
				}
				else {
					if (!getMainFrame()->writeCanvas(getPosition()+CPoint(curX+1,i),cursorStyle,"_")) return 3;					
				}
			}
		}
	}
	return 0;	
}

bool CInputCommand::readCommand(IRA::CString& input)
{
	if (getInput()) {
		input=IRA::CString(m_buffer);
		return true;
	}
	else {
		return false;
	}
}

int CInputCommand::parseCommand(IRA::CString *fields,int maxFields)
{
	IRA::CString input,Str;
	int i=0,n=0;
	if (readCommand(input)) {
		while (getNextToken((const char*)input,i,Str)) {
			fields[n]=Str;
			n++;
			if (n>=maxFields) return n;
		}
		return n;
	}
	else return 0;
}

bool CInputCommand::getInput()
{
	char ch;
	if (!isAlive() || (!getEnabled())) return false;
	if (!getMainFrame()->readCanvas(ch)) return false;
	else if (ch==KEY_LEFTARROW) {
		if (m_bufferScroller>0) {
			m_bufferScroller--;
			Refresh();
		}
		return false;
	}
	else if (ch==KEY_RIGHTARROW) {
		if (m_bufferScroller<m_bufferCounter) {
			m_bufferScroller++;
			Refresh();
		}
		return false;
	}
	else if (ch==KEY_UPPERARROW) {
		if (!isEmpty()) {
			if (m_stackScroller==MAX_COMMAND_NUMBER) {
				m_stackScroller=m_stackPointer;
			}
			else if (m_stackScroller>0) {
				m_stackScroller--;
			}		
			strcpy(m_buffer,m_inputStack[m_stackScroller]);
			m_bufferCounter=strlen(m_buffer);
			m_bufferScroller=m_bufferCounter;
			setValue(IRA::CString(m_buffer));
			Refresh();
		}
		return false;
	}
	else if (ch==KEY_LOWERARROW) {
		if (!isEmpty()) {
			if (m_stackScroller==MAX_COMMAND_NUMBER) {
				m_bufferCounter=0;
				m_bufferScroller=0;
				setValue("");
				Refresh();
			}
			else if (m_stackScroller<m_stackPointer) {
				m_stackScroller++;
				strcpy(m_buffer,m_inputStack[m_stackScroller]);
				m_bufferCounter=strlen(m_buffer);
				m_bufferScroller=m_bufferCounter;
				setValue(IRA::CString(m_buffer));
				Refresh();
			}
			else {
				m_stackScroller=MAX_COMMAND_NUMBER;
				m_bufferCounter=0;
				m_bufferScroller=0;
				setValue("");
				Refresh();
			}
		}
		return false;
	}
	else if (ch==KEY_ENTER) {
		m_buffer[m_bufferCounter]=0;
		if (isEmpty()) { //the stack is empty
			m_stackPointer=0;
		}
		else {
			if (m_stackPointer<MAX_COMMAND_NUMBER-1) m_stackPointer++;
			else { // the first input (the last in the stack) is dismissed and replaced by the  
				for (int i=0;i<m_stackPointer;i++) {
					strcpy(m_inputStack[i],m_inputStack[i+1]);
				}
			}
		}
		strcpy(m_inputStack[m_stackPointer],m_buffer);
		m_stackScroller=MAX_COMMAND_NUMBER;
		m_bufferCounter=0;
		m_bufferScroller=0;
		setValue("");
		Refresh();
		return true;
	}
	else if (ch==KEY_BACKSPACE) {
		if (m_bufferScroller>0) {
			if (m_bufferScroller<m_bufferCounter) {
				for (int i=m_bufferScroller-1;i<m_bufferCounter-1;i++) { //need to make rooms for the new character
					m_buffer[i]=m_buffer[i+1];
				}				
			}
			m_bufferCounter--;
			m_bufferScroller--;
			m_buffer[m_bufferCounter]=0;
			setValue(IRA::CString(m_buffer));
			Refresh();
		}
		return false;
	}
	else {
		if (m_bufferCounter==0) bzero(m_buffer,m_textLength+1);
		if (m_bufferCounter<m_textLength) {	
			if (m_bufferScroller<m_bufferCounter) {
				for (int i=m_bufferCounter-1;i>=m_bufferScroller;i--) { //need to make rooms for the new character
					m_buffer[i+1]=m_buffer[i];
				}
			}
			m_buffer[m_bufferScroller]=ch;
			m_bufferScroller++;
			m_bufferCounter++;
			/*IRA::CString temp;  // used to read the ascii code of a character
			temp.Format("%03d",ch);
			m_buffer[m_bufferScroller]=temp[0];
			m_bufferScroller++;
			m_bufferCounter++;
			m_buffer[m_bufferScroller]=temp[1];
			m_bufferScroller++;
			m_bufferCounter++;
			m_buffer[m_bufferScroller]=temp[2];
			m_bufferScroller++;
			m_bufferCounter++;*/
			m_buffer[m_bufferCounter]=0;
			setValue(IRA::CString(m_buffer));
			Refresh();
		}
		return false;
	}
}

bool CInputCommand::getNextToken(const char *Msg,int &Start,IRA::CString &Ret)
{
	int i;
	char *App;
	i=strlen(Msg);
	App=new char[i+1];
	i=0;
	while (Msg[Start]!=0) {
		if ((Msg[Start]==m_delimiter)) {
			Start++;
			break;
		}
		else {
			App[i]=Msg[Start];
			Start++;
			i++;
		}
	}
	App[i]=0;
	Ret=IRA::CString(App);
	delete []App;
	if (i==0) return false;
	else return true;
}

bool CInputCommand::isEmpty() const
{
	return (m_stackPointer==MAX_COMMAND_NUMBER);
}
