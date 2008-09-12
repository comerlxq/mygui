/*!
	@file
	@author		Albert Semenov
	@date		01/2008
	@module
*/
#ifndef __MYGUI_PROGRESS_FACTORY_H__
#define __MYGUI_PROGRESS_FACTORY_H__

#include "MyGUI_Prerequest.h"
#include "MyGUI_WidgetFactoryInterface.h"
#include "MyGUI_WidgetDefines.h"

namespace MyGUI
{
	namespace factory
	{

		class _MyGUIExport ProgressFactory : public WidgetFactoryInterface
		{
		public:
			ProgressFactory();
			~ProgressFactory();

			// ���������� ���������� �������
			const std::string& getType();
			WidgetPtr createWidget(const std::string& _skin, const IntCoord& _coord, Align _align, CroppedRectangleInterface * _parent, WidgetCreator * _creator, const std::string& _name);

			// ������ ��� ��������
			void Progress_Range(WidgetPtr _widget, const std::string &_key, const std::string &_value);
			void Progress_Position(WidgetPtr _widget, const std::string &_key, const std::string &_value);
			void Progress_AutoTrack(WidgetPtr _widget, const std::string &_key, const std::string &_value);
			void Progress_StartPoint(WidgetPtr _widget, const std::string &_key, const std::string &_value);
		};

	} // namespace factory
} // namespace MyGUI

#endif // __MYGUI_PROGRESS_FACTORY_H__
