



struct noncopyable{

	noncopyable() = default;
	virtual ~noncopyable() = default;
	noncopyable(const noncopyable&) = delete;
	noncopyable& operator(const noncopyable&) = delete;
};
