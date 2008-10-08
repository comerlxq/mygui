/*!
	@file
	@author		Albert Semenov
	@date		01/2008
	@module
*/
#include "MyGUI_Progress.h"
#include "MyGUI_WidgetSkinInfo.h"
#include "MyGUI_Widget.h"
#include "MyGUI_Gui.h"
#include "MyGUI_SkinManager.h"

namespace MyGUI
{

	MYGUI_RTTI_CHILD_IMPLEMENT( Progress, Widget );

	const size_t PROGRESS_AUTO_WIDTH = 200;
	const size_t PROGRESS_AUTO_RANGE = 1000;
	const float PROGRESS_AUTO_COEF = 400;

	Progress::Progress(const IntCoord& _coord, Align _align, const WidgetSkinInfoPtr _info, ICroppedRectangle * _parent, IWidgetCreator * _creator, const Ogre::String & _name) :
		Widget(_coord, _align, _info, _parent, _creator, _name),
		mTrackWidth(1),
		mTrackStep(0),
		mRange(0), mEndPosition(0), mStartPosition(0),
		mAutoTrack(false),
		mFillTrack(false),
		mStartPoint(Align::Left),
		mTrackMin(0)
	{
		for (VectorWidgetPtr::iterator iter=mWidgetChild.begin(); iter!=mWidgetChild.end(); ++iter) {
			if (*(*iter)->_getInternalData<std::string>() == "Client") {
				MYGUI_DEBUG_ASSERT( ! mWidgetClient, "widget already assigned");
				mWidgetClient = (*iter);
			}
		}
		if (null == mWidgetClient) mWidgetClient = this;

		const MapString & properties = _info->getProperties();
		MapString::const_iterator iterS = properties.find("TrackSkin");
		if (iterS != properties.end()) mTrackSkin = iterS->second;
		iterS = properties.find("TrackWidth");
		if (iterS != properties.end()) mTrackWidth = utility::parseInt(iterS->second);
		iterS = properties.find("TrackMin");
		if (iterS != properties.end()) mTrackMin = utility::parseInt(iterS->second);
		if (1 > mTrackWidth) mTrackWidth = 1;
		iterS = properties.find("TrackStep");
		if (iterS != properties.end()) mTrackStep = utility::parseInt(iterS->second);
		else mTrackStep = mTrackWidth;
		iterS = properties.find("TrackFill");
		if (iterS != properties.end()) mFillTrack = utility::parseBool(iterS->second);
		iterS = properties.find("StartPoint");
		if (iterS != properties.end()) setProgressStartPoint(Align::parse(iterS->second));

	}

	Progress::~Progress()
	{
		//Gui::getInstance().removeFrameListener(newDelegate(this, &Progress::frameEntered));
	}

	void Progress::setProgressRange(size_t _range)
	{
		if (mAutoTrack) return;
		mRange = _range;
		if (mEndPosition > mRange) mEndPosition = mRange;
		if (mStartPosition > mRange) mStartPosition = mRange;
		updateTrack();
	}

	void Progress::setProgressPosition(size_t _pos)
	{
		if (mAutoTrack) return;
		mEndPosition = _pos;
		if (mEndPosition > mRange) mEndPosition = mRange;
		updateTrack();
	}

	void Progress::setProgressAutoTrack(bool _auto)
	{
		if (mAutoTrack == _auto) return;
		mAutoTrack = _auto;

		if (mAutoTrack) {
			Gui::getInstance().eventFrameStart += newDelegate(this, &Progress::frameEntered);
			mRange = PROGRESS_AUTO_RANGE;
			mEndPosition = mStartPosition = 0;
			mAutoPosition = 0.0f;
		}
		else {
			Gui::getInstance().eventFrameStart -= newDelegate(this, &Progress::frameEntered);
			mRange = mEndPosition = mStartPosition = 0;
		}
		updateTrack();
	}

	void Progress::frameEntered(float _time)
	{
		if (false == mAutoTrack) return;
		mAutoPosition += (PROGRESS_AUTO_COEF * _time);
		size_t pos = (size_t)mAutoPosition;

		if (pos > (mRange + PROGRESS_AUTO_WIDTH)) mAutoPosition = 0.0f;

		if (pos > mRange) mEndPosition = mRange;
		else mEndPosition = mAutoPosition;

		if (pos < PROGRESS_AUTO_WIDTH) mStartPosition = 0;
		else mStartPosition = pos - PROGRESS_AUTO_WIDTH;

		updateTrack();
	}

	void Progress::setPosition(const IntCoord& _coord)
	{
		updateTrack();
		Widget::setPosition(_coord);
	}

	void Progress::setSize(const IntSize& _size)
	{
		updateTrack();
		Widget::setSize(_size);
	}

	void Progress::setProgressFillTrack(bool _fill)
	{
		mFillTrack = _fill;
		updateTrack();
	}

	void Progress::updateTrack()
	{
		// ��� ������
		if ((0 == mRange) || (0 == mEndPosition)) {
			for (VectorWidgetPtr::iterator iter=mVectorTrack.begin(); iter!=mVectorTrack.end(); ++iter) {
				(*iter)->hide();
			}
			return;
		}

		// ��� ������� �������
		if (mFillTrack) {

			if (mVectorTrack.empty()) {
				WidgetPtr widget = mWidgetClient->createWidget<Widget>(mTrackSkin, IntCoord(), Align::Left | Align::VStretch);
				mVectorTrack.push_back(widget);
			}
			else {
				// ������ ���������� � ������ ���� �����
				VectorWidgetPtr::iterator iter=mVectorTrack.begin();
				(*iter)->show();
				(*iter)->setAlpha(ALPHA_MAX);

				// ��� ������� �� ������� ��������
				++iter;
				for (; iter!=mVectorTrack.end(); ++iter) {
					(*iter)->hide();
				}
			}

			WidgetPtr wid = mVectorTrack.front();

			// ��������� �����
			if ((0 == mStartPosition) && (mRange == mEndPosition)) {
				setTrackPosition(wid, 0, 0, getClientWidth(), getClientHeight());
			}
			// ��
			else {
				int pos = (int)mStartPosition * (getClientWidth() - mTrackMin) / (int)mRange;
				setTrackPosition(wid, pos, 0, ((int)mEndPosition * (getClientWidth() - mTrackMin) / (int)mRange) - pos + mTrackMin, getClientHeight());
			}

			return;
		}

		// ������� ��������� ������� ��� �����
		int width = getClientWidth();
		int count = width / mTrackStep;
		int ost = (width % mTrackStep);
		if (ost > 0) {
			width += mTrackStep - ost;
			count ++;
		}

		while ((int)mVectorTrack.size() < count) {
			WidgetPtr widget = mWidgetClient->createWidget<Widget>(mTrackSkin, IntCoord(/*(int)mVectorTrack.size() * mTrackStep, 0, mTrackWidth, getClientHeight()*/), Align::Left | Align::VStretch);
			widget->hide();
			mVectorTrack.push_back(widget);
		}

		// ��� �����
		if ((0 == mStartPosition) && (mRange == mEndPosition)) {
			int pos = 0;
			for (VectorWidgetPtr::iterator iter=mVectorTrack.begin(); iter!=mVectorTrack.end(); ++iter) {
				(*iter)->setAlpha(ALPHA_MAX);
				(*iter)->show();
				setTrackPosition(*iter, pos * mTrackStep, 0, mTrackWidth, getClientHeight());
				pos++;
			}
		}
		// ��, �������� �������
		else {
			// ������� �� �����
			int hide_pix = (width * (int)mStartPosition / (int)mRange);
			int hide = hide_pix / mTrackStep;
			// ������� �����
			int show_pix = (width * (int)mEndPosition / (int)mRange);
			int show = show_pix / mTrackStep;

			int pos = 0;
			for (VectorWidgetPtr::iterator iter=mVectorTrack.begin(); iter!=mVectorTrack.end(); ++iter) {
				if (0 > show) {
					(*iter)->hide();
				}
				else if (0 == show) {
					(*iter)->setAlpha((float)(show_pix % mTrackStep) / (float)mTrackStep);
					(*iter)->show();
					setTrackPosition(*iter, pos * mTrackStep, 0, mTrackWidth, getClientHeight());
				}
				else {
					if (0 < hide) {
						(*iter)->hide();
					}
					else if (0 == hide) {
						(*iter)->setAlpha(1.0f - ((float)(hide_pix % mTrackStep) / (float)mTrackStep));
						(*iter)->show();
						setTrackPosition(*iter, pos * mTrackStep, 0, mTrackWidth, getClientHeight());
					}
					else {
						(*iter)->setAlpha(ALPHA_MAX);
						(*iter)->show();
						setTrackPosition(*iter, pos * mTrackStep, 0, mTrackWidth, getClientHeight());
					}
				}
				hide --;
				show --;
				pos ++;
			}
		}
	}

	void Progress::setTrackPosition(WidgetPtr _widget, int _left, int _top, int _width, int _height)
	{
		if (mStartPoint.isLeft()) _widget->setPosition(_left, _top, _width, _height);
		else if (mStartPoint.isRight()) _widget->setPosition(mWidgetClient->getWidth() - _left - _width, _top, _width, _height);
		else if (mStartPoint.isTop()) _widget->setPosition(_top, _left, _height, _width);
		else if (mStartPoint.isBottom()) _widget->setPosition(_top, mWidgetClient->getHeight() - _left - _width, _height, _width);
	}

	void Progress::setProgressStartPoint(Align _align)
	{
		if ((_align == Align::Left) || (_align == Align::Right) || (_align == Align::Top) || (_align == Align::Bottom)) {
			mStartPoint = _align;
		}
		else {
			mStartPoint = Align::Left;
			MYGUI_LOG(Warning, "Progress bar support only Left, Right, Top or Bottom align values");
		}
		updateTrack();
	}

} // namespace MyGUI
