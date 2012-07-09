/*!
	@file
	@author		Albert Semenov
	@date		07/2012
*/
#include "Precompiled.h"
#include "ListBoxItemControl.h"
#include "Localise.h"

namespace tools
{
	void ListBoxItemControl::getCellDimension(MyGUI::Widget* _sender, MyGUI::IntCoord& _coord, bool _drop)
	{
		int height = MyGUI::utility::parseValue<int>(_sender->getUserString("CellHeight"));
		_coord.set(0, 0, _sender->getClientCoord().width, height);
	}

	ListBoxItemControl::ListBoxItemControl(MyGUI::Widget* _parent) :
		wraps::BaseCellView<SkinItem*>("ListBoxItemControl.layout", _parent)
	{
		mMainWidget->setCoord(0, 0, _parent->getWidth(), _parent->getHeight());
		mMainWidget->setAlign(MyGUI::Align::Stretch);
		mMainWidget->setNeedMouseFocus(false);
	}

	void ListBoxItemControl::update(const MyGUI::IBDrawItemInfo& _info, SkinItem* _data)
	{
		if (_data == nullptr)
			return;

		MyGUI::Button* button = mMainWidget->castType<MyGUI::Button>(false);
		if (button == nullptr)
			return;

		if (_data->getUniqum())
			button->setCaption(_data->getName());
		else
			button->setCaption(replaceTags("ColourError") + _data->getName());

		std::string state = "";
		if (_info.select)
		{
			if (_info.active)
			{
				state = "highlighted_checked";
			}
			else
			{
				state = "normal_checked";
			}
		}
		else
		{
			if (_info.active)
			{
				state = "highlighted";
			}
			else
			{
				state = "normal";
			}
		}

		if (_info.drag)
		{
			if (_info.drop_accept)
				state = "highlighted_checked";
			else if (_info.drop_refuse)
				state = "normal";
		}
		else
		{
			if (!_info.active)
			{
				if (_info.drop_accept)
					state = "highlighted";
				else if (_info.drop_refuse)
					state = "normal";
			}
		}

		mMainWidget->_setWidgetState(state);
	}
}
