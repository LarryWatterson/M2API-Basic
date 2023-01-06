// find

	BindPacketInfo(pkPacketInfo);


// add below
#ifdef ENABLE_INPUT_API
	m_pInputAPI = std::shared_ptr<CInputAPI>(new CInputAPI());
#endif

// find

		if (bHeader == HEADER_CG_TEXT)

// change if statement with that
#ifdef ENABLE_INPUT_API
	if (bHeader == HEADER_CG_TEXT)
	{
		++c_pData;
		const char * c_pSep;

		if (!(c_pSep = strchr(c_pData, '\n')))
			return -1;

		if (*(c_pSep - 1) == '\r')
			--c_pSep;

		std::string stBuf;
		stBuf.assign(c_pData, 0, c_pSep - c_pData);

		sys_log(0, "SOCKET_CMD: FROM(%s) CMD(%s)", d->GetHostName(), stBuf.c_str());
		
		std::shared_ptr<CInputAPI> api = GetInputAPI();
		if (!api) {
			sys_err("No API was open to receive the CMD request!");
			return (c_pSep - c_pData) + 1;
		}

		api->SetDesc(d);
		api->Process(stBuf);

		return (c_pSep - c_pData) + 1;
	}
#else
	[[[ .... ]]]
#endif