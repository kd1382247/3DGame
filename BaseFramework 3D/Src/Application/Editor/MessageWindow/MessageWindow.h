#pragma once

class MessageWindow
{
public:


	MessageWindow(){}
	~MessageWindow(){}
	

	void Open(const std::string&title,const std::string&message);
	void Draw();




private:

	std::string m_title;
	std::string m_message;

	bool        m_requestOpen = false;

};