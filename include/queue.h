class Queue
{
public:
	static bool Open(string &path, string &name, Queue **que);
	
	Queue(){
	}
	~Queue() {
	
	}
	virtual bool Read(string &str) = 0;
	virtual bool Write(char *str, uint64_t length)=0;
	virtual uint32_t Size() = 0;
};

