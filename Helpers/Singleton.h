#ifndef H__SINGLETONE
#define H__SINGLETONE

template <class T>
class CSingleton
{
public:
	virtual ~CSingleton()
	{
		auto pInstance = m_pInstance;
		m_pInstance = nullptr;

		SAFE_DELETE(pInstance);
	};

	template <typename... Args>
	static T* GetInstance(Args... args)
	{
		if (!m_pInstance)
		{
			m_pInstance = new T(std::forward<Args>(Args)...);
		}

		return m_pInstance;
	};

	static void DestroyInstance()
	{
		auto pInstance = m_pInstance;
		m_pInstance = nullptr;

		SAFE_DELETE(pInstance);
	};

private:
	static T *m_pInstance;
};

template <class T> T *CSingleton<T>::m_pInstance = nullptr;

#endif