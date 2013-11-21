/*****************************************************************************************************  */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: TW_InputCommand.h,v 1.3 2008-06-11 10:43:44 a.orlati Exp $																							        */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                				    When                    What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it)     11/04/2006       Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it)     11/06/2008       Added command stack                   */


#ifndef _TW_INPUTCOMMAND_H
#define _TW_INPUTCOMMAND_H

#include "TW_FrameComponent.h"

namespace TW {
	
/**
 * This class implements a component as an input command line. This component allows the GUI to accept user inputs taht are
 * returned as strings or string tokens.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 	
*/	
class CInputCommand : protected virtual CFrameComponent
{
public:
	/**
	 * Constructor.
	*/
	CInputCommand();

	/**
	 * Destructor.
	*/
	virtual ~CInputCommand();

	/**
	 * Use this method to know the maximum number of characters for the user input.
    * @return the maximum input length
	*/
	inline WORD getTextLength() const { return m_textLength; }
	
	/**
	 * This method can be used to set the maximum lenght of the user input. 
	 * @param len number of character allowed for the user input.	 
	*/
	void setTextLength(const WORD& len);
	
	/**
	 * Sets the token delimiter used by <i>parseCommand()</i> to devide the user input into tokens.
	 * @param delimiter the character the separates the tokens.
	*/
	void setDelimiter(const char& delimiter=' ') { m_delimiter=delimiter; }
	
	/** 
	 * @return the delimiter character.
	*/
	char getDelimiter() const  { return m_delimiter; }
	
	/**
	 * This method check if there is a new input and in case it returns the string containing the user input.
	 * @param input it will contain the sequence of character typed by the user.
	 * @return true if a new user input is ready, false otherwise
	*/
	bool readCommand(IRA::CString& input);
	
	/**
	 * This method behaves like <i>readCommand()</i> does. I.e. it informs the caller if a new user input is ready. 
	 * It also parses the user input looking for tokens, each token is then returned in a sequence of strings.
	 * Tokens are splitted by identifying a delimiter character. By default the delimiter character is a space.
	 * @param fields this must point to an array of strings (allocated by the caller), that will store the parsed tokens.
	 * @param maxFields the size of the array of strings.
	 * @return the number of string returned inside the array. A zero means that the user input is not ready yet.
	*/
	int parseCommand(IRA::CString *fields,int maxFields);

	using CFrameComponent::setPosition;
	using CFrameComponent::getPosition;
	using CFrameComponent::setTag;
	using CFrameComponent::getTag;
	using CFrameComponent::setStyle;
	using CFrameComponent::getStyle;
	using CFrameComponent::setWidth;
	using CFrameComponent::getWidth;
	using CFrameComponent::setHeight;
	using CFrameComponent::getHeight;
	//using CFrameComponent::setHAlign;
	//using CFrameComponent::getHAlign;
	//using CFrameComponent::setWAlign;
	//using CFrameComponent::getWAlign;
	using CFrameComponent::setOutputComponent;
	using CFrameComponent::getOutputComponent;
	using CFrameComponent::setEnabled;
	using CFrameComponent::getEnabled;
	//using CFrameComponent::setValue;
	using CFrameComponent::getValue;
	using CFrameComponent::Refresh;
		
protected:
	/** Length of the buffer that stores the input from the user. */
	WORD m_textLength;
	/** Pointer to the buffer that stores the user input */
	//char *m_buffer;
	/** Buffer counter */
	//WORD m_counter;
	/** Delimiter character used to split the user input into tokens */
	char m_delimiter;
	//IRA::CString m_lastInput;
	/** The command stack */
	char **m_inputStack;
	/** Points to the last inserted input in the stack */
	WORD m_stackPointer;
	/** Points to the position of the input stack when scrolling with the up arrow */
	WORD m_stackScroller;
	/** currently edited buffer */
	char *m_buffer;
	/** Counts the number of valid characters inside the buffer */
	WORD m_bufferCounter;
	/** points to the current position of the cursor inside the buffer */
	WORD m_bufferScroller;
	/**
	 * @return true if a new input string is ready.
	*/
	bool getInput();
	/**
    * Gets the next token inside a string. 
	 * @return false if no more tokens could be found.
	*/
	bool getNextToken(const char *Msg,int &Start,IRA::CString &Ret);
	/** Override the base class draw method */
	WORD draw();
private:
	bool isEmpty() const;
};


}


#endif
