#pragma once

//------cpp library------
#include <string>
//------cpp library------

namespace Pixel {
	//------material's custom parameter------
	class MaterialCustomPara
	{
	public:
		MaterialCustomPara() = default;

		virtual ~MaterialCustomPara();

		//shader parameter name
		std::string ConstValueName;

		//------in terms of the parameter name to check------
		MaterialCustomPara& operator=(const MaterialCustomPara& Para)
		{
			ConstValueName = Para.ConstValueName;
			return *this;
		}

		friend bool operator>(const MaterialCustomPara& Para1, const MaterialCustomPara& Para2)
		{
			return Para1.ConstValueName > Para2.ConstValueName;
		}

		friend bool operator<(const MaterialCustomPara& Para1, const MaterialCustomPara& Para2)
		{
			return Para1.ConstValueName < Para2.ConstValueName;
		}

		friend bool operator==(const MaterialCustomPara& Para1, const MaterialCustomPara& Para2)
		{
			return Para1.ConstValueName == Para2.ConstValueName;
		}
		//------in terms of the parameter name to check------
	};
	//------material's custom parameter------
}