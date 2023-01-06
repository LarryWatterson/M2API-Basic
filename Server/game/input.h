// find

void LoginFailure(LPDESC d, const char* c_pszStatus);

// add above

#ifdef ENABLE_INPUT_API
class CInputAPI
{
public:
	CInputAPI();

	void SetDesc(LPDESC d) { m_desc = d; }
	void CloseDesc();
	void Process(std::string msg);

private:
	bool IsAuthorized();

private:
	LPDESC m_desc;
};
#endif

// find

	CPacketInfo* m_pMainPacketInfo;

// add below

	private:
		std::shared_ptr<CInputAPI> GetInputAPI() { return m_pInputAPI; }
		std::shared_ptr<CInputAPI> m_pInputAPI;