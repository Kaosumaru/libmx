#pragma once
#include <memory>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

class thread_pool
{
private:
	boost::asio::io_service io_service_;
	std::unique_ptr<boost::asio::io_service::work> work_;
	boost::thread_group threads_;
	std::size_t available_;
	boost::mutex mutex_;
public:

	/// @brief Constructor.
	thread_pool(std::size_t pool_size)
		: work_(std::make_unique<boost::asio::io_service::work>(io_service_)),
		available_(pool_size)
	{
		for (std::size_t i = 0; i < pool_size; ++i)
		{
			threads_.create_thread(std::bind(&boost::asio::io_service::run,
				&io_service_));
		}
	}

	/// @brief Destructor.
	~thread_pool()
	{
		// Force all threads to return from io_service::run().
		io_service_.stop();

		// Suppress all exceptions.
		try
		{
			threads_.join_all();
		}
		catch (...) {}
	}

	/// @brief Adds a task to the thread pool if a thread is currently available.
	void run_task(const boost::function< void() > &task)
	{
		// Post a wrapped task into the queue.
		io_service_.post(task);
	}

	void join()
	{
		boost::unique_lock< boost::mutex > lock(mutex_);
		work_.reset();
		threads_.join_all();
	}

private:
	/// @brief Wrap a task so that the available count can be increased once
	///        the user provided task has completed.
	void wrap_task(boost::function< void() > task)
	{
		// Run the user supplied task.
		try
		{
			task();
		}
		// Suppress all exceptions.
		catch (...) {}
	}
};

#endif
