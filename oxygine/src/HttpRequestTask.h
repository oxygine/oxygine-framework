#pragma once
#include "oxygine_include.h"
#include "AsyncTask.h"
#include <vector>
#include <string>

namespace oxygine
{
	DECLARE_SMART(HttpRequestTask, spHttpRequestTask);
	class HttpRequestTask: public AsyncTask
	{
	public:
		static spHttpRequestTask create();

		static void init();
		static void release();	
		
		/**dispatching AsyncTask events*/
		enum
		{
			ERROR = AsyncTask::ERROR,
			PROGRESS = AsyncTask::PROGRESS,
			COMPLETE = AsyncTask::COMPLETE
		};

		class ProgressEvent : public Event
		{
		public:
			enum {EVENT=PROGRESS};
			ProgressEvent(int Loaded, int Total) :Event(PROGRESS), loaded(Loaded), total(Total){};

			int loaded;
			int total;
		};

		HttpRequestTask();
		~HttpRequestTask();

		const std::vector<unsigned char>&	getResponse() const;
		const std::string&					getFileName() const;

		void getResponseSwap(std::vector<unsigned char> &);

		void setPostData(const std::vector<unsigned char> &data);
		void setUrl(const std::string &url);
		void setFileName(const std::string &name);


	protected:
		void _prerun();

		//async
		void progress(int loaded, int total);		

		void _onCustom(const ThreadMessages::message &msg);
		void dispatchProgress(int loaded, int total);

        virtual void _setFileName(const std::string &name){}
        virtual void _setUrl(const std::string &url){}
        virtual void _setPostData(const std::vector<unsigned char> &data){}

		std::string _url;
		std::string _fname;
		std::vector<unsigned char> _response;
		std::vector<unsigned char> _postData;

		//size_t _loaded;
		//size_t _total;
	};
}