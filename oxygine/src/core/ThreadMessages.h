#pragma once
#include "oxygine_include.h"
#include <vector>
#include "pthread.h"
namespace oxygine
{
	class ThreadMessages
	{
	public:
		struct message
		{
			void *arg1;
			void *arg2;
			unsigned int id;
		};

		ThreadMessages();
		~ThreadMessages();

		bool empty();
		void wait();
		void get(message &ev);
		bool peek(message &ev, bool del);
		void reply(void *val);
		void *send(void *arg1, void *arg2);
		void post(void *arg1, void *arg2);

	private:
		std::vector<message> _events;
		unsigned int _id;
		unsigned int _waitReplyID;
		unsigned int _lastGetID;
		bool _replied;
		void* _reply;
		pthread_cond_t _cond;
		pthread_mutex_t _mutex;
	};
}