class BankingDiscordObject_Video : BankingDiscordObject
{
	private int m_width;
	private int m_height;

	void BankingDiscordObject_Video(string url, int width = -1, int height = -1)
	{
		m_width = width;
		m_height = height;
		m_fields.Set("url", url);
	}

	override string DefaultJSONConvert()
	{
		return ConvertToJsonCloseByObject("video");
	}
	
	override protected string ConvertSpecialFields()
	{
		string addedFieldsToJson = "";
		
		addedFieldsToJson += GetWidthField();
		addedFieldsToJson += GetHeightField();
		return addedFieldsToJson;
	}
	
	private string GetWidthField()
	{
		if (m_width > -1)
		{
			return BankingDiscordHelper.GetJSONLineOthers("width", m_width.ToString());
		}
		return "";
	}
	
	private string GetHeightField()
	{
		if (m_height > -1)
		{
			return BankingDiscordHelper.GetJSONLineOthers("height", m_height.ToString());
		}
		return "";
	}

	override protected bool CanSetKey(string key)
	{
		switch (key)
		{
			case "url":
				return true;
		}
		return false;
	}
};