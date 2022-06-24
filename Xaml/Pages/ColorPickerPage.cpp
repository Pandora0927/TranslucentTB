#include "pch.h"

#include "ColorPickerPage.h"
#if __has_include("Pages/ColorPickerPage.g.cpp")
#include "Pages/ColorPickerPage.g.cpp"
#endif

#include "appinfo.hpp"
#include "util/hstring_format.hpp"

namespace winrt::TranslucentTB::Xaml::Pages::implementation
{
	ColorPickerPage::ColorPickerPage(txmp::TaskbarState state, Windows::UI::Color originalColor) : m_State(state), m_OriginalColor(originalColor)
	{ }

	void ColorPickerPage::InitializeComponent()
	{
		ComponentConnectorT::InitializeComponent();

		// TODO: localize
		Title(Util::hstring_format<L"{} - Color picker - " APP_NAME>(GetTextForState(m_State)));
	}

	bool ColorPickerPage::CanMove() noexcept
	{
		return !m_DialogOpened;
	}

	bool ColorPickerPage::RequestClose()
	{
		if (m_DialogOpened)
		{
			return false;
		}

		if (Picker().Color() != m_OriginalColor)
		{
			OpenConfirmDialog();
			return false;
		}
		else
		{
			Close();
			return true;
		}
	}

	void ColorPickerPage::OkButtonClicked(const IInspectable &, const wux::RoutedEventArgs &)
	{
		m_ChangesCommittedHandler(Picker().Color());
		Close();
	}

	void ColorPickerPage::CancelButtonClicked(const IInspectable &, const wux::RoutedEventArgs &)
	{
		RequestClose();
	}

	void ColorPickerPage::DialogOpened(const IInspectable &, const wuxc::ContentDialogOpenedEventArgs &) noexcept
	{
		m_DialogOpened = true;
	}

	void ColorPickerPage::DialogClosed(const IInspectable &, const wuxc::ContentDialogClosedEventArgs &) noexcept
	{
		m_DialogOpened = false;
	}

	Windows::UI::Color ColorPickerPage::OriginalColor() noexcept
	{
		return m_OriginalColor;
	}

	void ColorPickerPage::PickerColorChanged(const muxc::ColorPicker &, const muxc::ColorChangedEventArgs &args)
	{
		m_ColorChangedHandler(args.NewColor());
	}

	std::wstring_view ColorPickerPage::GetTextForState(txmp::TaskbarState state)
	{
		switch (state)
		{
			using enum txmp::TaskbarState;

		// TODO: in the future, when localization is possible, we should lookup the same resource used for the context menu entries.
		case Desktop: return L"Desktop";
		case VisibleWindow: return L"Visible window";
		case MaximisedWindow: return L"Maximised window";
		case StartOpened: return L"Start opened";
		case SearchOpened: return L"Search opened";
		case TaskViewOpened: return L"Task View opened";
		case BatterySaver: return L"Battery saver";
		default: throw std::invalid_argument("Unknown taskbar state");
		}
	}

	fire_and_forget ColorPickerPage::OpenConfirmDialog()
	{
		const auto self_weak = get_weak();
		const auto result = co_await ConfirmCloseDialog().ShowAsync();

		if (const auto self = self_weak.get())
		{
			if (result != wuxc::ContentDialogResult::None)
			{
				if (result == wuxc::ContentDialogResult::Primary)
				{
					self->m_ChangesCommittedHandler(self->Picker().Color());
				}

				self->Close();
			}
		}
	}
}
