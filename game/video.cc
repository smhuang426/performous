#include "video.hh"

#include "util.hh"
#include <cmath>

Video::Video(std::string const& _videoFile, double videoGap): m_mpeg(true, false, _videoFile), m_videoGap(videoGap), m_surfaceTime(), m_lastTime(), m_alpha(-0.5, 1.5) {}

void Video::prepare(double time) {
	time += m_videoGap;
	VideoFrame& fr = m_videoFrame;
	// Time to switch frame?
	if (!fr.data.empty() && time >= fr.timestamp) {
		Bitmap bitmap;
		bitmap.fmt = pix::RGB;
		bitmap.buf.swap(fr.data);
		bitmap.resize(fr.width, fr.height);
		m_surface.load(bitmap);
		m_surfaceTime = fr.timestamp;
	}
	// Preload the next future frame
	if (fr.data.empty()) while (m_mpeg.videoQueue.tryPop(fr) && fr.timestamp < time) {};
	// Do a seek before next render, if required
	if (time < m_lastTime - 1.0 || (!fr.data.empty() && time > fr.timestamp + 7.0)) {
		m_mpeg.seek(std::max(0.0, time - 5.0));  // -5 to workaround ffmpeg inaccurate seeking
		fr.data.clear();
	}
	m_lastTime = time;
}

void Video::render(double time) {
	double tdist = std::abs(m_surfaceTime - time);
	m_alpha.setTarget(tdist < 0.4 ? 1.2f : -0.5f);
	float alpha = clamp(m_alpha.get());
	if (alpha > 0.0f) {
		Color color;
		if (alpha < 1.0f) {
			color = Color(alpha, alpha, alpha, alpha);
		} else {
			color = Color(1.0f, 1.0f, 1.0f);
		}
		{
			ColorTrans c(color);
			m_surface.draw();
		}
	}
}

