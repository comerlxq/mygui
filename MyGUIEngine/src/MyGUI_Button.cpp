/*!
	@file
	@author		Albert Semenov
	@date		11/2007
	@module
*/
#include "MyGUI_Button.h"
#include "MyGUI_WidgetSkinInfo.h"
#include "MyGUI_StaticImage.h"
#include "MyGUI_CastWidget.h"
#include "MyGUI_InputManager.h"

namespace MyGUI
{

	Ogre::String Button::WidgetTypeName = "Button";

	Button::Button(const IntCoord& _coord, Align _align, const WidgetSkinInfoPtr _info, CroppedRectangleInterface * _parent, WidgetCreator * _creator, const Ogre::String & _name) :
		Widget(_coord, _align, _info, _parent, _creator, _name),
		mIsMousePressed(false),
		mIsMouseFocus(false),
		mIsStateCheck(false),
		mImage(null),
		mModeCheck(false)
	{

		// ������ ��������
		const MapString & properties = _info->getProperties();
		if (!properties.empty()) {
			MapString::const_iterator iter = properties.find("ButtonPressed");
			if (iter != properties.end()) setButtonPressed(iter->second == "true");
			iter = properties.find("StateCheck");
			if (iter != properties.end()) setStateCheck(iter->second == "true");
		}

		for (VectorWidgetPtr::iterator iter=mWidgetChild.begin(); iter!=mWidgetChild.end(); ++iter) {
			if ((*iter)->_getInternalString() == "Image") {
				MYGUI_DEBUG_ASSERT( ! mImage, "widget already assigned");
				mImage = castWidget<StaticImage>(*iter);
			}
		}

		// ��������� �� ������ ����� �������� ������� ������
		mModeCheck = mStateInfo.find("normal_check") != mStateInfo.end();

	}

	void Button::_onMouseSetFocus(WidgetPtr _old)
	{
		_setMouseFocus(true);
		// !!! ����������� �������� � ����� ������
		Widget::_onMouseSetFocus(_old);
	}

	void Button::_onMouseLostFocus(WidgetPtr _new)
	{
		_setMouseFocus(false);
		// !!! ����������� �������� � ����� ������
		Widget::_onMouseLostFocus(_new);
	}

	void Button::_onMouseButtonPressed(int _left, int _top, MouseButton _id)
	{
		if (_id == MB_Left) {
			mIsMousePressed = true;
			updateButtonState();
		}
		// !!! ����������� �������� � ����� ������
		Widget::_onMouseButtonPressed(_left, _top, _id);
	}

	void Button::_onMouseButtonReleased(int _left, int _top, MouseButton _id)
	{
		if (_id == MB_Left) {
			mIsMousePressed = false;
			updateButtonState();
		}
		// !!! ����������� �������� � ����� ������
		Widget::_onMouseButtonReleased(_left, _top, _id);
	}

	void Button::setImageIndex(size_t _index)
	{
		if (mImage) mImage->setImageNum(_index);
	}

	size_t Button::getImageIndex()
	{
		if (mImage) return mImage->getImageNum();
		return ITEM_NONE;
	}

	void Button::setEnabled(bool _enabled)
	{
		if (mEnabled == _enabled) return;
		mEnabled = _enabled;

		for (VectorWidgetPtr::iterator iter = mWidgetChild.begin(); iter != mWidgetChild.end(); ++iter) {
			(*iter)->setEnabled(_enabled);
		}

		updateButtonState();

		if ( ! mEnabled) {
			InputManager::getInstance()._unlinkWidget(this);
			mIsMouseFocus = false;
		}
	}

	void Button::updateButtonState()
	{
		if (mModeCheck) {
			if (mIsStateCheck) {
				if ( ! mEnabled) setState("disable_check");
				else if (mIsMousePressed) setState("pressed_check");
				else if (mIsMouseFocus) setState("active_check");
				else setState("normal_check");
			}
			else {
				if ( ! mEnabled) setState("disable");
				else if (mIsMousePressed) setState("pressed");
				else if (mIsMouseFocus) setState("active");
				else setState("normal");
			}
		}
		else {
			if ( ! mEnabled) {
				setState("disable");
			}
			else {
				if (mIsMouseFocus) {
					if (mIsMousePressed || mIsStateCheck) setState("select");
					else setState("active");
				} else {
					if (mIsMousePressed || mIsStateCheck) setState("pressed");
					else setState("normal");
				}
			}
		}
	}

} // namespace MyGUI
