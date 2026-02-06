#ifndef PREYDATOR_THREAD_POOL_H__
#define PREYDATOR_THREAD_POOL_H__

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstddef>

class ThreadPool
{
public:
	explicit ThreadPool(size_t thread_count);
	~ThreadPool();
	void enqueue(std::function<void()> job);
	void wait();

private:
	void workerLoop();

	std::vector<std::thread> m_workers;
	std::queue<std::function<void()>> m_jobs;
	std::mutex m_mtx;
	std::condition_variable m_cv;
	std::atomic<bool> m_stop;
	std::mutex m_wait_mtx;
	std::condition_variable m_wait_cv;
	std::atomic<int> m_active_jobs{0};
};

#endif /* PREYDATOR_THREAD_POOL_H__ */