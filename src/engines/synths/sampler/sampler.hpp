#pragma once

#include "core/engine/engine.hpp"

#include "util/soundfile.hpp"

#include "util/iterator.hpp"

#include "list"

namespace otto::engines {

  using namespace core;
  using namespace core::engine;
  using namespace props;

  struct Sample {
    struct iterator;

    Sample(fs::path file);
    Sample() = default;

    auto size() const noexcept;

    iterator begin(float stride = 1) const;

    iterator end(float stride = 1) const;

    util::sequence<std::vector<float>::const_iterator, std::vector<float>::const_iterator>
    waveform() const;

    /// Audio samples per entry in @ref waveform()
    int waveform_scale() const;

    int start_point() const;
    int end_point() const;
    int loop_start() const;
    int loop_end() const;

    int start_point(int val);
    int end_point(int val);
    int loop_start(int val);
    int loop_end(int val);

    struct iterator : util::iterator_facade<iterator, float, std::forward_iterator_tag> {
      using vector_iterator = std::vector<float>::const_iterator;

      iterator(const Sample& sample, int index, float stride = 1.f);
      iterator(const iterator&);
      iterator& operator=(const iterator&);

      void advance(std::ptrdiff_t d);
      float dereference() const;
      bool equal(const iterator& rhs) const;

      int index() const;

      const Sample& sample;
      bool do_loop = false;

    private:
      int start_point() const;
      int end_point() const;
      int loop_start() const;
      int loop_end() const;
      int signed_index() const;

      int _index = 0;
      float _playback_speed = 1.f;
      float _error = 0.f;
    };

    bool cut = false;
    bool loop = false;

  private:
    std::vector<float> _audio_data;
    std::vector<float> _waveform;
    int _size = 0;

    friend struct iterator;

    int _start_point = 0;
    int _end_point = 0;
    int _loop_start = -1;
    int _loop_end = -1;

    /// The normal speed. When reading a sample from a file, this is the AudioManager's
    /// samplerate divided by the sample's
    float speed_modifier = 1.f;
  };

  struct Sampler : SynthEngine, EngineWithEnvelope {
    struct Props : Properties<> {
      Property<std::string> file = {this, "FILENAME", ""};
      Property<float> filter = {this, "FILTER", 0, has_limits::init(0, 1), steppable::init(0.01)};
      Property<float> speed = {this, "SPEED", 1, has_limits::init(-10, 10), steppable::init(0.01)};
    } props;

    Sampler();

    void restart();

    float operator()() noexcept;

    audio::ProcessData<1> process(audio::ProcessData<1>) override;

    ui::Screen& envelope_screen() override;
    ui::Screen& voices_screen() override;
  private:
    friend struct SamplerScreen;
    friend struct SamplerEnvelopeScreen;

    void load_file(fs::path file);
    Sample sample;
    Sample::iterator play_position = sample.end();

    std::unique_ptr<ui::Screen> _envelope_screen;
  };

} // namespace otto::engines
