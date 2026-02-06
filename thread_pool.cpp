#include "thread_pool.h"


ThreadPool::ThreadPool(size_t thread_count)
	: m_stop(false)
{
	for (size_t i = 0; i < thread_count; ++i)
	{
		m_workers.emplace_back([this] { workerLoop(); });
	}
}

ThreadPool::~ThreadPool()
{
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		m_stop = true;
	}
	m_cv.notify_all();
	for (auto& worker : m_workers)
	{
		if (worker.joinable()) worker.join();
	}
}

void ThreadPool::enqueue(std::function<void()> job)
{
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		m_jobs.push(std::move(job));
	}
	m_cv.notify_one();
}

void ThreadPool::wait()
{
	std::unique_lock<std::mutex> lk(m_wait_mtx);
	m_wait_cv.wait(lk, [&] { return m_active_jobs == 0 && m_jobs.empty(); });
}

void ThreadPool::workerLoop()
{
	while (true)
	{
		std::function<void()> job;
		{
			std::unique_lock<std::mutex> lk(m_mtx);
			m_cv.wait(lk, [&] { return m_stop || !m_jobs.empty(); });

			if (m_stop && m_jobs.empty()) return;

			job = std::move(m_jobs.front());
			m_jobs.pop();
			++m_active_jobs;
		}
		job();
		{
			std::lock_guard<std::mutex> lk(m_wait_mtx);
			--m_active_jobs;
		}
		m_wait_cv.notify_one();
	}
}
