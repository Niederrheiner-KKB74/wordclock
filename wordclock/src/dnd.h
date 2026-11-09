#ifndef WORDCLOCK_DND_H
#define WORDCLOCK_DND_H

class DND {
  public:
    // true during the configured night time (regardless of the night mode)
    static bool active(int hour, int minute);

    // Night mode "switch off": clock is dark.
    static bool dark(int hour, int minute);

    // Night mode "dim" / "dim with night color": clock stays on, dimmed.
    static bool dimmed(int hour, int minute);
};

#endif
