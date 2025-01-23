#pragma once
#include "pch.h"
#include "Components/Components/Utils.hpp"


namespace GUI
{
	void open_link(const char* url);
	void open_link(const wchar_t* url);


	struct ImageLink
	{
		const wchar_t* link;
		const char* tooltip;
		std::shared_ptr<ImageWrapper> image;
		ImVec2 image_size;

		ImageLink() {}
		ImageLink(std::shared_ptr<ImageWrapper> img) { init(); }
		ImageLink(std::shared_ptr<ImageWrapper> img, const wchar_t* url, const char* tooltip = "") : image(img), link(url), tooltip(tooltip) { init(); }


		void init()
		{
			auto size = image->GetSizeF();
			image_size = {size.X, size.Y};
		}


		bool size_is_zero()
		{
			return image_size.x == 0 || image_size.y == 0;
		}

		void scale_to_desired_height(float target_height)
		{
			Vector2F size = image->GetSizeF();

			if (size.Y <= 0) return;

			const float scale_factor = target_height / size.Y;		//		x / 100		=	target_height / og_width

			size *= scale_factor;

			image_size = { size.X, size.Y };
		}

		void scale_to_desired_width(float target_width)
		{
			Vector2F size = image->GetSizeF();

			if (size.X <= 0) return;

			const float scale_factor = target_width / size.X;		//		x / 100		=	target_height / og_width

			size *= scale_factor;

			image_size = { size.X, size.Y };
		}


		void display(const float target_height = 25.0f)
		{
			if (!image) return;

			if (auto img_tex = image->GetImGuiTex())
			{
				if (size_is_zero())
				{
					scale_to_desired_height(target_height);
				}

				ImGui::Image(img_tex, image_size);

				if (ImGui::IsItemHovered())
				{
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
					ImGui::SetTooltip(tooltip);
				}
				if (ImGui::IsItemClicked())
				{
					open_link(link);
				}
			}
		}
	};


	struct FooterLinks
	{
		ImageLink github;
		ImageLink discord;
		ImageLink youtube;

		void set_height(const float height)
		{
			github.scale_to_desired_height(height);
			discord.scale_to_desired_height(height);
			youtube.scale_to_desired_height(height);
		}
	};

	static constexpr float footer_icon_height =		25.0f;

	static constexpr const wchar_t* discord_link =	L"https://discord.gg/tHZFsMsvDU";
	static constexpr const char* discord_desc =		"Need help? Join the discord";

	static constexpr const wchar_t* youtube_link =	L"https://www.youtube.com/@SSLowRL";
	static constexpr const char* youtube_desc =		"YouTube page";



	namespace Colors
	{
		struct Color
		{
			float r, g, b, a;

			Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
			
			// Initialize with float values
			Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

			Color(const ImVec4& color) : r(color.w), g(color.x), b(color.y), a(color.z) {}

			// Initialize with an FLinearColor
			Color(const FLinearColor& color) : r(color.R), g(color.G), b(color.B), a(color.A) {}

			// Initialize with a int32_t color (0xAARRGGBB format)
			Color(int32_t color)
			{
				a = ((color >> 24) & 0xFF)	/ 255.0f;	// Extract and normalize Alpha
				r = ((color >> 16) & 0xFF)	/ 255.0f;	// Extract and normalize Red
				g = ((color >> 8) & 0xFF)	/ 255.0f;	// Extract and normalize Green
				b = (color & 0xFF)			/ 255.0f;	// Extract and normalize Blue
			}

			// Initialize with an FColor
			Color(const FColor& color)
			{
				r = color.R / 255.0f; // Normalize Red
				g = color.G / 255.0f; // Normalize Green
				b = color.B / 255.0f; // Normalize Blue
				a = color.A / 255.0f; // Normalize Alpha
			}



			ImVec4 GetImGuiColor() const
			{
				return ImVec4{ r, g, b, a };
			}

			FLinearColor GetLinearColor() const
			{
				return FLinearColor{ r, g, b, a };
			}

			int32_t GetIntColor() const
			{
				FColor col = GetFColor();

				return	(static_cast<int32_t>(col.A) << 24)	|
						(static_cast<int32_t>(col.R) << 16)	|
						(static_cast<int32_t>(col.G) << 8)	|
						static_cast<int32_t>(col.B);
			}

			FColor GetFColor() const
			{
				FColor col;

				col.R = static_cast<uint8_t>(std::round(r * 255.0f));
				col.G = static_cast<uint8_t>(std::round(g * 255.0f));
				col.B = static_cast<uint8_t>(std::round(b * 255.0f));
				col.A = static_cast<uint8_t>(std::round(a * 255.0f));

				return col;
			}
		};


		extern const ImVec4 White;
		extern const ImVec4 Red;
		extern const ImVec4 Green;
		extern const ImVec4 Blue;
		
		extern const ImVec4 Yellow;
		extern const ImVec4 BlueGreen;
		extern const ImVec4 Pinkish;
		
		extern const ImVec4 LightBlue;
		extern const ImVec4 LightRed;
		extern const ImVec4 LightGreen;

		extern const ImVec4 DarkGreen;

		extern const ImVec4 Gray;
	}



	void ClickableLink(const char* label, const char* url, const ImVec4& textColor = ImVec4(1, 1, 1, 1), ImVec2 size = ImVec2(0, 0));

	void Spacing(int amount = 1);
	void SameLineSpacing_relative(float horizontalSpacingPx);
	void SameLineSpacing_absolute(float horizontalSpacingPx);


	void SettingsHeader(const char* id, const char* pluginVersion, const ImVec2& size, bool showBorder = false);

	void display_footer_links(FooterLinks& footer_links, float horizontal_spacing_between_links_px = 20.0f);
	
	void SettingsFooter(const char* id, const ImVec2& size, FooterLinks& footer_links, bool showBorder = false);

	void OldSettingsFooter(const char* id, const ImVec2& size, bool showBorder = false);
}