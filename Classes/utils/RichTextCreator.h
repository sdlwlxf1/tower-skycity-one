#ifndef __RICH_TEXT_CREATOR_H__
#define __RICH_TEXT_CREATOR_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CCSGUIReader.h"
#include "StringHandler.h"
#include "ui/UIRichText.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;

static inline RichText* createRichTextWithStr(const std::string& contentStr)
{

	auto richText = RichText::create();
	ValueVector content;
	ValueVector vec1;
	stringToValueVector(contentStr,"###",vec1);
	for (auto i:vec1)
	{
		strArray a;
		split(i.asString(),"---",a);
		ValueVector pair;

		ValueMap map1;
		stringToValueMap(a.at(0),map1);
		pair.push_back(cocos2d::Value(map1));
		if(a.size() > 1)
		{
			ValueMap map2;
			stringToValueMap(a.at(1),map2);
			pair.push_back(cocos2d::Value(map2));
		}
		content.push_back(cocos2d::Value(pair));
	}

	int tag = 0;
	for(auto i : content)
	{
		auto vec = i.asValueVector();
		auto map1 = vec.at(0).asValueMap();
		auto typeV = map1["type"];
		std::string type = "Text";
		if(!typeV.isNull())
		{
			type = typeV.asString();
		}
		if(type == "Text")
		{
			RichElementText* element = nullptr;
			std::string text = "";
			Color3B color = Color3B::WHITE;
			GLubyte opacity = 255;
			Vec2 anchorPoint = Vec2(0.5,0.5);
			std::string fontName = "Marker Felt";
			float fontSize = 20;
			if(vec.size() > 1)
			{
				auto map2 = vec.at(1).asValueMap();
				auto colorV = map2["color"];
				auto opacityV = map2["opacity"];
				auto anchorPointV = map2["anchorPointV"];
				auto fontNameV = map2["fontName"];
				auto fontSizeV = map2["fontSize"];
				auto textV = map2["text"];
				if(!textV.isNull())
				{
					text = textV.asString();
				}
				if(!colorV.isNull())
				{
					color = stringToColor3B(colorV.asString());
				}
				if(!opacityV.isNull())
				{
					opacity = opacityV.asInt();
				}
				if(!fontNameV.isNull())
				{
					fontName = fontNameV.asString();
				}
				if(!fontSizeV.isNull())
				{
					fontSize = fontSizeV.asFloat();
				}
			}
			element  = RichElementText::create(tag, color, opacity, text, fontName, fontSize);
			richText->pushBackElement(element);
		}
		else if(type == "Image")
		{
			RichElementImage* element = nullptr;
			Color3B color = Color3B::WHITE;
			GLubyte opacity = 255;
			std::string filePath = "";
			Vec2 anchorPoint = Vec2(0.5,0.5);
			if(vec.size() > 1)
			{
				auto map2 = vec.at(1).asValueMap();
				auto colorV = map2["color"];
				auto opacityV = map2["opacity"];
				auto anchorPointV = map2["anchorPoint"];
				auto filePathV = map2["filePath"];
				if(!colorV.isNull())
				{
					color = stringToColor3B(colorV.asString());
				}
				if(!opacityV.isNull())
				{
					opacity = opacityV.asInt();
				}
				if(!filePathV.isNull())
				{
					filePath = filePathV.asString();
				}
			}
			element  = RichElementImage::create(tag, color, opacity, filePath);
			richText->pushBackElement(element);
		}
		tag++;
	}
	return richText;
}

#endif /*__RICH_TEXT_CREATOR_H__*/
