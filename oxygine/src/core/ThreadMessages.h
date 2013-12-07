#pragma once
#include "oxygine_include.h"
#include <vector>
#include "pthread.h"
namespace oxygine
{
	class ThreadMessages
	{
	public:		
		struct message;
		typedef void (*callback)(const message &m);
		struct message
		{
			message():msgid(0), arg1(0), arg2(0), cb(0), cbData(0), _id(0), _result(0), _replied(false){}
			int msgid;
			void *arg1;
			void *arg2;			
			callback cb;
			void *cbData;

			unsigned int _id;
			void *_result;
			bool _replied;
		};

		ThreadMessages();
		~ThreadMessages();

		bool empty();
		void wait();
		void get(message &ev);
		bool peek(message &ev, bool del);
				
		void*send(int msgid, void *arg1, void *arg2);
		void sendCallback(int msgid, void *arg1, void *arg2, callback cb, void *cbData);
		void post(int msgid, void *arg1, void *arg2);

		void reply(void *val);		

	private:
		void _replyLast(void *val);
		std::vector<message> _events;
		unsigned int _id;
		unsigned int _waitReplyID;
		//unsigned int _lastGetID;
		message _last;
		//bool _replied;
		pthread_cond_t _cond;
		pthread_mutex_t _mutex;
	};
}