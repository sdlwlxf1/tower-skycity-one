#ifndef __SINGLETON_H__
#define __SINGLETON_H__

template <typename T>
class Singleton
{
public:
	inline static T* getInstance();
	inline static void release();
private:
	static T* t;
};

template <typename T>
inline T* Singleton<T>::getInstance()
{
	if (!t)
	{
		t = new T;
	}
	return t;
}

template<typename T>
inline void Singleton<T>::release()
{
	if (t)
	{
		delete t;
		t = 0;
	}
}

template <typename T> 
T* Singleton<T>::t = 0;

#endif // __SINGLE_H__