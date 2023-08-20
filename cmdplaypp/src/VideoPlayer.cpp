#include "VideoPlayer.hpp"
#include "ConsoleUtils.hpp"
#include <iostream>

cmdplay::VideoPlayer::VideoPlayer(const std::string& filePath, const std::string& brightnessLevels, int windowWidth, int windowHeight) :
	m_filePath(filePath), m_brightnessLevels(brightnessLevels), m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
	m_asciifier = std::make_unique<Asciifier>(brightnessLevels, windowWidth, windowHeight);
	m_decoder = std::make_unique<video::FfmpegDecoder>();
}

void cmdplay::VideoPlayer::LoadVideo()
{
	m_decoder->LoadVideo(m_filePath, m_windowWidth, m_windowHeight);
	m_audioSource = std::make_unique<audio::AudioSource>(m_filePath);
}

void cmdplay::VideoPlayer::Enter()
{
	m_audioSource->Play();
	while (true)
	{
		m_decoder->SetPlaybackPosition(m_audioSource->GetPlaybackPosition() + PREBUFFER_TIME);

		auto nextFrame = m_decoder->GetNextFrame();
		if (nextFrame == nullptr)
			continue;

		while (nextFrame->m_time > m_audioSource->GetPlaybackPosition());
		SetConsoleTitle(std::to_wstring(nextFrame->m_time).c_str());
		cmdplay::ConsoleUtils::SetCursorPosition(0, 0);
		std::cout << m_asciifier->BuildFrame(nextFrame->m_data);

		delete nextFrame;
	}
}
