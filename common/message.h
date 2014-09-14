#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstring>
#include <iostream>
#include <streambuf>
#include <vector>
#include <sys/time.h>


template<typename CharT, typename TraitsT = std::char_traits<CharT> >
class vectorwrapbuf : public std::basic_streambuf<CharT, TraitsT> 
{
public:
    vectorwrapbuf(std::vector<CharT> &vec) 
	{
        this->setg(vec.data(), vec.data(), vec.data() + vec.size());
    }
};


struct membuf : public std::basic_streambuf<char>
{
    membuf(char* begin, char* end) 
	{
        this->setg(begin, begin, end);
    }
};


enum message_type
{
	base = 0,
	header = 1,
	payload = 2,
	sampleformat = 3,
	serversettings = 4,
	timemsg = 5
};



struct tv
{
	tv()
	{
		timeval t;
		gettimeofday(&t, NULL);
		sec = t.tv_sec;
		usec = t.tv_usec;
	}
	tv(timeval tv) : sec(tv.tv_sec), usec(tv.tv_usec) {};
	tv(int32_t _sec, int32_t _usec) : sec(_sec), usec(_usec) {};

	int32_t sec;
	int32_t usec;
};



struct BaseMessage
{
	BaseMessage() : type(base), id(0), refersTo(0)
	{
	}

	BaseMessage(message_type type_) : type(type_), id(0), refersTo(0)
	{
	}

	virtual ~BaseMessage()
	{
	}

	virtual void read(std::istream& stream)
	{
		stream.read(reinterpret_cast<char*>(&type), sizeof(uint16_t));
		stream.read(reinterpret_cast<char*>(&id), sizeof(uint16_t));
		stream.read(reinterpret_cast<char*>(&refersTo), sizeof(uint16_t));
		stream.read(reinterpret_cast<char *>(&sent.sec), sizeof(int32_t));
		stream.read(reinterpret_cast<char *>(&sent.usec), sizeof(int32_t));
		stream.read(reinterpret_cast<char *>(&received.sec), sizeof(int32_t));
		stream.read(reinterpret_cast<char *>(&received.usec), sizeof(int32_t));
		stream.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
	}

	void deserialize(char* payload)
	{
		membuf databuf(payload, payload + BaseMessage::getSize());
		std::istream is(&databuf);
		read(is);
	}

	void deserialize(const BaseMessage& baseMessage, char* payload)
	{
		type = baseMessage.type;
		id = baseMessage.id;
		refersTo = baseMessage.refersTo;
		sent = baseMessage.sent;
		received = baseMessage.received;
		size = baseMessage.size;
		membuf databuf(payload, payload + size);
		std::istream is(&databuf);
		read(is);
	}

	virtual void serialize(std::ostream& stream)
	{
		stream.write(reinterpret_cast<char*>(&type), sizeof(uint16_t));
		stream.write(reinterpret_cast<char*>(&id), sizeof(uint16_t));
		stream.write(reinterpret_cast<char*>(&refersTo), sizeof(uint16_t));
		stream.write(reinterpret_cast<char *>(&sent.sec), sizeof(int32_t));
		stream.write(reinterpret_cast<char *>(&sent.usec), sizeof(int32_t));
		stream.write(reinterpret_cast<char *>(&received.sec), sizeof(int32_t));
		stream.write(reinterpret_cast<char *>(&received.usec), sizeof(int32_t));
		size = getSize();
		stream.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));
		doserialize(stream);
	}

	virtual uint32_t getSize()
	{
		return 3*sizeof(uint16_t) + 2*sizeof(tv) + sizeof(uint32_t);
	};

	uint16_t type;
	uint16_t id;
	uint16_t refersTo;
	tv sent;
	tv received;
	uint32_t size;

protected:
	virtual void doserialize(std::ostream& stream)
	{
	};
};


#endif

