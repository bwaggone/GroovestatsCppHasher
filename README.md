# GrooveStats C++ Hasher

This is a utility that's intended to be passed in a "Songs" directory and spit out groovestats hashes.

See the following for an example of how it prints:

```
Beginning Recursive Read through: PATH_TO_SONGS
PATH_TO_SONGS/15gays1pack/1UL [StarrySergal]/1UL.ssc
Style: dance-single     Difficulty: Beginner    Hash : c19809b40759f7c5
PATH_TO_SONGS/15gays1pack/1UL [StarrySergal]/1UL.ssc
Style: dance-single     Difficulty: Easy        Hash : e536c8f6bbfbd457
PATH_TO_SONGS/15gays1pack/1UL [StarrySergal]/1UL.ssc
Style: dance-single     Difficulty: Medium      Hash : 9784aeb857a16756
PATH_TO_SONGS/15gays1pack/1UL [StarrySergal]/1UL.ssc
Style: dance-single     Difficulty: Hard        Hash : 67aed01cbc1f436a
PATH_TO_SONGS/15gays1pack/1UL [StarrySergal]/1UL.ssc
Style: dance-single     Difficulty: Challenge   Hash : 408dfb4fbcb3dcd5
PATH_TO_SONGS/15gays1pack/1UL [StarrySergal]/1UL.ssc
Style: dance-single     Difficulty: Edit        Hash : ce9f28e1fb3271f9
```

The program takes one argument, which is the path to the Songs directory. It
only works on SSC and SM files. It expects a strict directory structure,
(potentially TODO to make it less restrictive)

The program is intended to load song data as closely to ITGm as possible,
though exceptions to its processes are documented.

For a quick list of differences:
  * Skipping the RageFile library entirely
  * Massively simplified SSC and SM loading, not using a separate class.
  * No NoteData, TimingSegment, or other "low level" song based data structures.
  * Massively simplified Steps and Song classes.
  * s/RString/std::string
