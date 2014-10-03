/* Copyright 2013-2014, Dominik Schnitzer <dominik@schnitzer.at>
 *                2014, Jan Schlueter <jan.schluter@ofai.at>
 *
 * This file is part of Musly, a program for high performance music
 * similarity computation: http://www.musly.org/.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/**
\mainpage Usage

Using the Musly library interface in your application is straightforward : 0. .

 1. include the musly.h header in your project
 2. initialize musly: musly_jukebox_poweron()
 3. analyze some music: musly_track_analyze_audiofile()
 4. initialize the similarity component with musly_jukebox_setmusicstyle() and
    musly_jukebox_addtracks().
 5. compute similarities and playlists: musly_jukebox_similarity()
 6. deinitialize musly: musly_jukebox_poweroff()
 
A more detailed description of the libary calls and parameters can be found
in musly.h. The source code distribution also includes a sample application
(musly/main.cpp). The demo app can be used to try and evaluate the Musly
similarity measures. It is our reference implementation.
*/

#ifndef MUSLY_H_
#define MUSLY_H_

#include <musly/musly_types.h>

#ifdef WIN32
  /** \hideinitializer */
  #define MUSLY_EXPORT __declspec(dllexport)
#else
  /** \hideinitializer */
  #define MUSLY_EXPORT __attribute__ ((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/** Return the version of Musly.
 * \returns the version as a null terminated string.
 */
MUSLY_EXPORT const char*
musly_version();


/** Set the musly debug level. Valid levels are 0 (Quiet, DEFAULT), 1 (Error),
 * 2 (Warning), 3 (Info), 4 (Debug), 5 (Trace). All output will be sent to
 * stderr.
 *
 * \param[in] level The musly library debug level, if the level is invalid it
 * will be set to the closest valid level.
 */
MUSLY_EXPORT void
musly_debug(
        int level);


/** Lists all available music similarity methods. The methods are returned as
 * a single null terminated string. The methods are separated by a comma (,).
 * Use a method name to power on a Musly jukebox.
 *
 * \returns all available methods
 *
 * \sa musly_jukebox_poweron()
 */
MUSLY_EXPORT const char*
musly_jukebox_listmethods();


/** Lists all available audio file decoders. The decoders are returned as
 * a single null terminated string. The decoders are separated by a comma (,).
 * Use a decoder name to power on a Musly jukebox musly_jukebox_poweron()
 * The decoders are used in musly_track_analyze_audiofile().
 *
 * \returns all available audio file decoders.
 *
 * \sa musly_jukebox_poweron()
 */
MUSLY_EXPORT const char*
musly_jukebox_listdecoders();


/** Describe the initialized method. This call describes the used music
 * similarity method of the referenced musly_jukebox in more detail.
 *
 * \param[in] jukebox an initialized reference to a Musly jukebox.
 * \returns a description of the currently initialized music similarity
 * method as a null terminated string.
 *
 * \sa musly_jukebox_poweron()
 */
MUSLY_EXPORT const char*
musly_jukebox_aboutmethod(
        musly_jukebox* jukebox);


/** Returns a reference to an initialized Musly jukebox object. To initialize
 * Musly you need to specify a music similarity method to use and a decoder.
 * You can set both values to 0 (NULL) to initialize the default method and
 * decoder. To list all available music similarity methods use
 * musly_jukebox_listmethods(). To list all available audio file decoders
 * use musly_jukebox_listdecoders(). If the initialization fails, NULL
 * is returned. To get more information about the initialized music similarity
 * method use musly_jukebox_aboutmethod().
 *
 * The returned reference is required for almost all subsequent calls to Musly
 * library calls. To add a music track to the jukebox inventory use
 * musly_jukebox_addtracks(). To compute recommendations with the jukebox use
 * musly_jukebox_similarity(). Note that before adding tracks or computing
 * similarities, the music style needs to be set with
 * musly_jukebox_setmusicstyle().
 *
 * \param[in] method the desired music similarity method.
 * \param[in] decoder the desired decoder to initialize.
 * \returns a reference to an initialized Musly jukebox object.
 *
 * \sa musly_jukebox_poweroff(), musly_jukebox_addtracks(),
 * musly_jukebox_setmusicstyle(), musly_jukebox_similarity()
 */
MUSLY_EXPORT musly_jukebox*
musly_jukebox_poweron(
        const char* method,
        const char* decoder);


/** Deinitializes the given Musly jukebox. The referenced method and decoder
 * objects are freed. Previously allocated Musly tracks allocated with
 * musly_track_alloc() need to be freed separately. The referenced Musly
 * jukebox object is invalidated by this call.
 *
 * \param[in] jukebox the Musly jukebox to deinitialize.
 *
 * \sa musly_jukebox_poweron()
 */
MUSLY_EXPORT void
musly_jukebox_poweroff(
        musly_jukebox* jukebox);


/** Initialize the jukebox music style. To properly use the similarity function
 * it is necessary to give the algorithms a hint about the music we are working
 * with. Do this by passing a random sample of the tracks you want to analyze.
 * As a rule of thumb use a maximum of 1000 randomly selected tracks to set
 * the music style. The tracks are analyzed and copied to internal storage as
 * needed, so you may safely deallocate the given array and tracks after
 * the call.
 *
 * \note
 * This must be called before adding any tracks to the jukebox. If you change
 * the music style of a filled jukebox, you need to re-register all existing
 * tracks via musly_jukebox_addtracks(), otherwise tracks added after the style
 * change will not be properly compared to tracks added before the change.
 *
 * \param[in] jukebox the Musly jukebox to set the music stlye.
 * \param[in] tracks a random sample array of Musly tracks to use for
 * the initialization.
 * \param[in] num_tracks the number of Musly tracks.
 *
 * \sa musly_jukebox_poweron(), musly_track_analyze_pcm(),
 * musly_track_analyze_audiofile()
 */
MUSLY_EXPORT int
musly_jukebox_setmusicstyle(
        musly_jukebox* jukebox,
        musly_track** tracks,
        int num_tracks);


/** Add tracks to the Musly jukebox. To use the music similarity routines
 * each Musly track has to be added to a jukebox. Internally Musly allocates an
 * initialization vector for each track computed with the tracks passed to
 * musly_jukebox_setmusicstyle().
 *
 * \param[in] jukebox the Musly jukebox to add the tracks to.
 * \param[in] tracks an array of musly_track objects to add to the jukebox.
 * \param[in,out] trackids the track identifiers for the tracks, either
 * read or written depending on the \p generate_ids parameter.
 * \param[in] num_tracks the length of the \p tracks and \p trackids array.
 * \param[in] generate_ids controls whether ids are to be generated
 * automatically or given by the caller.
 * If nonzero, Musly will assign sequential identifiers starting from
 * <tt>musly_jukebox_maxtrackid(jukebox) + 1</tt> and write the ids to
 * \p trackids. If zero, Musly will assign the ids given in \p trackids,
 * replacing existing tracks for ids already used by the jukebox.
 *
 * \returns 0 on success -1 on an error. When an error is returned no
 * track was added to Musly.
 *
 * \sa musly_jukebox_removetracks(), musly_jukebox_trackcount(),
 * musly_jukebox_setmusicstyle(), musly_jukebox_similarity()
 */
MUSLY_EXPORT int
musly_jukebox_addtracks(
        musly_jukebox* jukebox,
        musly_track** tracks,
        musly_trackid* trackids,
        int num_tracks,
        int generate_ids);


/** Remove tracks from the Musly jukebox.
 *
 * \param[in] jukebox the Musly jukebox to remove the tracks from.
 * \param[in] trackids the track identifiers of the tracks to remove,
 * unknown identifiers will be silently ignored.
 * \param[in] num_tracks the length of the \p trackids array.
 *
 * \returns 0 on success -1 on an error.
 *
 * \sa musly_jukebox_addtracks(), musly_jukebox_trackcount()
 */
MUSLY_EXPORT int
musly_jukebox_removetracks(
        musly_jukebox* jukebox,
        musly_trackid* trackids,
        int num_tracks);


/** Returns the number of tracks currently registered with the Musly jukebox.
 * Along with musly_jukebox_maxtrackid(), this can be used for versioning the
 * state of a jukebox.
 *
 * \param[in] jukebox the Musly jukebox to query
 * \returns the number of tracks currently registered with \p jukebox, or -1
 * on an error
 *
 * \sa musly_jukebox_addtracks(), musly_jukebox_removetracks()
 */
MUSLY_EXPORT int
musly_jukebox_trackcount(
        musly_jukebox* jukebox);


/** Returns the largest track identifier ever registered with the Musly
 * jukebox. Along with musly_jukebox_trackcount(), this can be used for
 * versioning the state of a jukebox:
 * <tt>(maxtrackid+1).(maxtrackid+1-trackcount)</tt> is a two-element version
 * number that will never decrease in the lifetime of a jukebox, provided that
 * musly_jukebox_maxtrackid() always increases when adding a track.
 *
 * \param[in] jukebox the Musly jukebox to query
 * \return the largest trackid seen by \p jukebox, or -1 if it has not seen any
 *
 * \sa musly_jukebox_addtracks(), musly_jukebox_removetracks()
 */
MUSLY_EXPORT musly_trackid
musly_jukebox_maxtrackid(
        musly_jukebox* jukebox);


/** Returns the trackids of all tracks currently registered with the the Musly
 * jukebox. Use musly_jukebox_trackcount() to ask how many trackids will be
 * returned.
 *
 * \param[in] jukebox the Musly jukebox to query
 * \param[out] trackids the ids of all registered tracks
 * \return the number of track ids written, or -1 in case of an error
 *
 * \sa musly_jukebox_trackcount()
 */
MUSLY_EXPORT int
musly_jukebox_gettrackids(
        musly_jukebox* jukebox,
        musly_trackid* trackids);


/** Computes the similarity between a seed track and a list of other music
 * tracks. To compute similarities between two music tracks the following
 * steps have to been taken:
 *
 *  - initialize a musly_jukebox object with: musly_jukebox_poweron()
 *  - analyze audio files, e.g. with musly_track_analyze_audiofile()
 *  - set the music style of the jukebox by using a small random sample of
 *    the audio tracks analyzed: musly_jukebox_setmusicstyle()
 *  - add the audio tracks to the musly_jukebox: musly_jukebox_addtracks()
 *  - use this function to compute similarities.
 *
 * \param[in] jukebox The Musly jukebox to use.
 * \param[in] seed_track The seed track to compute similarities to
 * \param[in] seed_trackid The id of the seed track as returned by
 * musly_jukebox_addtracks().
 * \param[in] tracks An array of musly_track objects to compute the
 * similarities to.
 * \param[in] trackids An array of musly_trackids corresponding to the tracks
 * array. The musly_trackids are returned after adding them to the
 * musly_jukebox
 * \param[in] num_tracks the size of the tracks and trackids arrays
 * \param[out] similarities a preallocated float array to hold the computed
 * similarities.
 * \returns 0 on success and -1 on an error.
 *
 * \sa musly_jukebox_poweron(), musly_track_analyze_audiofile(),
 * musly_track_analyze_pcm(), musly_jukebox_setmusicstyle(),
 * musly_jukebox_addtracks()
 */
MUSLY_EXPORT int
musly_jukebox_similarity(
        musly_jukebox* jukebox,
        musly_track* seed_track,
        musly_trackid seed_trackid,
        musly_track** tracks,
        musly_trackid* trackids,
        int num_tracks,
        float* similarities);


/** Tries to guess the most similar neighbors to the given trackid. If
 * similarity measures implement this call, it is usually a very efficient
 * way to pre-filter the whole jukebox collection for possible matches
 * (neighbors) to the query song (seed). The musly_tracks do not have to be
 * loaded to memory to use that call. It operates solely on an index usually
 * built when adding the track to the jukebox (musly_jukebox_addtracks()). A
 * maximum of num_neighbors is written in the neighbors list of track ids.
 * The returned neighbors can be used to drastically reduce the number of input
 * tracks (and thus computation time) for musly_jukebox_similarity().
 * If the method is not implemented or all neighbors should be analyzed,
 * -1 is returned. In that case consider all musly_tracks as possible nearest
 * neighbors and thus as input to musly_jukebox_similarity().
 *
 * \param[in] jukebox An initialized Musly jukebox object with tracks added
 * through musly_jukebox_addtrack().
 * \param[in] seed The seed track id to search for its nearest neighbors.
 * \param[out] neighbors The neighbors will be written to this preallocated
 * array.
 * \param[in] num_neighbors The maximum number of neighbors to write to the
 * neighbors array.
 * \returns the number of neighbors found for the given seed trackid (success).
 * -1 is returned on a failure.
 *
 * \sa musly_jukebox_similarity(), musly_jukebox_addtrack().
 */
MUSLY_EXPORT int
musly_jukebox_guessneighbors(
        musly_jukebox* jukebox,
        musly_trackid seed,
        musly_trackid* neighbors,
        int num_neighbors);


/** Tries to guess the most similar neighbors to the given trackid. In contrast
 * to musly_jukebox_guessneighbors(), this version does not search for neighbor
 * candidates among all registered tracks of the jukebox, but allows to limit
 * the search to a list of tracks identified by their track ids. This can be
 * useful to incorporate external search results, e.g., using metadata.
 *
 * \note
 * musly_jukebox_guessneighbors_filtered() with \p num_limit_to set to zero is
 * exactly equivalent to musly_jukebox_guessneighbors().
 * musly_jukebox_guessneighbors_filtered() with \p limit_to set to a list of
 * all registered track ids will generally also return the same results as
 * musly_jukebox_guessneighbors(), but may be a lot slower due to additional
 * lookups and worse locality.
 *
 * \param[in] jukebox An initialized Musly jukebox object with tracks added
 * through musly_jukebox_addtrack().
 * \param[in] seed The seed track id to search for its nearest neighbors.
 * \param[out] neighbors The neighbors will be written to this preallocated
 * array.
 * \param[in] num_neighbors The maximum number of neighbors to write to the
 * neighbors array.
 * \param[in] limit_to A list of track ids to limit the search to
 * \param[in] num_limit_to The length of the \p limit_to array. If zero,
 * \p limit_to is ignored and all registered track ids are searched instead.
 * \returns the number of neighbors found for the given seed trackid (success).
 * -1 is returned on a failure.
 *
 * \sa musly_jukebox_guessneighbors(), musly_jukebox_similarity().
 */
MUSLY_EXPORT int
musly_jukebox_guessneighbors_filtered(
        musly_jukebox* jukebox,
        musly_trackid seed,
        musly_trackid* neighbors,
        int num_neighbors,
        musly_trackid* limit_to,
        int num_limit_to);


/**
 * Returns the size in bytes needed for serializing the jukebox state.
 *
 * \param[in] jukebox An initialized Musly jukebox object.
 * \param[in] header If nonzero, include the size needed for serializing
 * jukebox metadata. This size is dependent on the similarity measure and
 * can also depend on the internal jukebox state.
 * \param[in] num_tracks If greater than zero, include the size needed for
 * serializing the state of \p num_tracks registered tracks. If negative,
 * include the size needed for serializing the state of all currently
 * registered tracks. This size is dependent on the similarity measure only.
 * \returns The number of bytes needed to store the state information.
 *
 * \note This method gives the sizes of internal indices built when calling
 * musly_jukebox_setmusicstyle() and musly_jukebox_addtracks(). For
 * the size of musly_track objects, see musly_track_binsize().
 */
MUSLY_EXPORT int
musly_jukebox_binsize(
        musly_jukebox* jukebox,
        int header,
        int num_tracks);


/**
 * Serializes the jukebox state into a byte buffer and returns the number of
 * bytes written. Call musly_jukebox_binsize() to determine the required
 * buffer size, and musly_jukebox_frombin() to deserialize a jukebox state.
 *
 * \param[in] jukebox An initialized Musly jukebox object.
 * \param[out] buffer The buffer to write to.
 * \param[in] header If nonzero, write the jukebox metadata.
 * \param[in] num_tracks The number of registered tracks to write the jukebox
 * state for. If negative or too large, writes the state for all except the
 * first `skip_tracks` registered tracks.
 * \param[in] skip_tracks The number of tracks to skip. Must be in range
 * [0, get_trackcount()]. Must be 0 if \p header is nonzero.
 * \returns The number of bytes written to the buffer, or -1 in case of an
 * error.
 *
 * \note \p skip_tracks allows to serialize the jukebox state in small
 * portions. Set \p num_tracks to the number of track states you can handle
 * at once, then call musly_jukebox_tobin() repeatedly in a loop, setting
 * \p skip_tracks to the number of track states you have already serialized.
 *
 * \note This method serializes the internal indices built when calling
 * musly_jukebox_setmusicstyle() and musly_jukebox_addtracks(). For
 * serialization of musly_track objects, see musly_track_tobin().
 *
 * \sa musly_jukebox_tofile()
 */
MUSLY_EXPORT int
musly_jukebox_tobin(
        musly_jukebox* jukebox,
        unsigned char* buffer,
        int header,
        int num_tracks,
        int skip_tracks);


/**
 * Deserializes the jukebox state from a byte buffer and returns the number of
 * bytes read. Use this to restore a jukebox state previously saved with
 * musly_jukebox_tobin().
 *
 * \param[in] jukebox An initialized Musly jukebox object. This must have been
 * initialized with the same music similarity method as the jukebox the state
 * has been exported from, otherwise behavior is undefined.
 * \param[in] buffer The byte buffer to read from.
 * \param[in] header If nonzero, will read and restore the jukebox metadata.
 * \param[in] num_tracks If greater than zero, will read and restore the state
 * of \p num_tracks registered tracks. If negative, will read and restore the
 * state of all registered tracks (only possible if \p header is nonzero).
 * \returns The number of bytes read, or -1 in case of an error.
 *
 * \note This method deserializes the internal indices built when calling
 * musly_jukebox_setmusicstyle() and musly_jukebox_addtracks(). For
 * deserialization of musly_track objects, see musly_track_frombin().
 *
 * \sa musly_jukebox_fromfile()
 */
MUSLY_EXPORT int
musly_jukebox_frombin(
        musly_jukebox* jukebox,
        unsigned char* buffer,
        int header,
        int num_tracks);


/**
 * Serializes a jukebox state and writes it to a file.
 *
 * \param jukebox An initialized Musly jukebox object.
 * \param filename The name of the file to write to.
 * \returns The number of bytes written, or -1 in case of an error.
 *
 * \sa musly_jukebox_fromfile
 */
MUSLY_EXPORT int
musly_jukebox_tofile(
        musly_jukebox* jukebox,
        const char* filename);


/**
 * Restores a jukebox from a file written by musly_jukebox_tofile().
 *
 * \param filename The name of the file to read from.
 * \returns A reference to an initialized Musly jukebox object, or NULL in
 * case of an error.
 *
 * \note Currently, a file cannot be read on a platform of a different
 * architecture (integer size or byte order) than it was written with.
 */
MUSLY_EXPORT musly_jukebox*
musly_jukebox_fromfile(
        const char* filename);


/** Allocates a musly_track in memory. As the size of a musly_track varies for
 * each music similarity method, an initialized Musly jukebox object reference
 * needs to be passed argument. You need to free the allocated musly_track with
 * musly_track_free().
 *
 * \param[in] jukebox A reference to an initialized Musly jukebox object.
 * \returns An allocated musly_track float array.
 *
 * \sa musly_track_free()
 */
MUSLY_EXPORT musly_track*
musly_track_alloc(
        musly_jukebox* jukebox);


/** Frees a musly_track previously allocated with musly_track_alloc().
 *
 * \param[in] track The musly track you want to free.
 *
 * \sa musly_track_alloc()
 */
MUSLY_EXPORT void
musly_track_free(
        musly_track* track);


/** Returns the size of a musly_track in bytes. In case you want to allocate
 * the musly_track yourself, allocate the memory and cast the memory to a
 * musly_track. The size of each musly_track varies from music similarity
 * method to method, that is, the size depends on the method musly has been
 * initialized with (musly_jukebox_poweron()). For safe serializing and
 * deserializing of a musly_track to/from memory use musly_track_tobin() and
 * musly_track_frombin().
 *
 * \param[in] jukebox A reference to an initialized musly_jukebox object.
 *
 * \returns The number of bytes of a musly_track for the given musly_jukebox.
 *
 * \sa musly_jukebox_poweron(), musly_track_tobin(), musly_track_frombin().
 */
MUSLY_EXPORT int
musly_track_size(
        musly_jukebox* jukebox);


/** Returns the buffer size in bytes required to hold a musly_track. To
 * serialize a musly_track for platform-independent usage, call
 * musly_track_tobin(), otherwise just copy the memory directly.
 * The size returned is dependent on the music similarity method.
 *
 * \param[in] jukebox A reference to an initialized musly_jukebox object.
 *
 * \returns The required buffer size required to hold a musly_track.
 *
 * \sa musly_jukebox_poweron(), musly_track_tobin(), musly_track_frombin().
 */
MUSLY_EXPORT int
musly_track_binsize(
        musly_jukebox* jukebox);


/** Serializes a musly_track to a byte buffer. Use this method to store or
 * transmit a musly_track. musly_track_binsize() bytes will be written to
 * to_buffer. To deserialize a buffer use musly_track_frombin().
 *
 * \param[in] jukebox A reference to an initialized musly_jukebox object.
 * \param[in] from_track The track to serialize
 * \param[out] to_buffer The buffer receiving the serialized track. The buffer
 * needs to be preallocated with musly_track_binsize() bytes.
 *
 * \returns The number of bytes written (musly_track_binsize()) in case of
 * success, -1 in case an error occurred.
 *
 * \note This transforms the musly_track data from host byte order to
 * network byte order (which is big endian). If you do not need to transmit
 * the data across platforms, you can directly copy musly_track_binsize() bytes
 * from \p from_track to \p to_buffer, or not use a buffer at all.
 *
 * \sa musly_jukebox_poweron(), musly_track_binsize(), musly_track_frombin().
 */
MUSLY_EXPORT int
musly_track_tobin(
        musly_jukebox* jukebox,
        musly_track* from_track,
        unsigned char* to_buffer);


/** Deserializes a byte buffer to a musly_track. Use this method re-transform a
 * previously serialized byte buffer (musly_track_tobin()) to a musly_track.
 * From the buffer musly_track_binsize() bytes will be read.
 *
 * \param[in] jukebox A reference to an initialized musly_jukebox object.
 * \param[in] from_buffer the buffer to use for deserialization
 * \param[out] to_track the musyl_track to store the deserialized track.
 *
 * \returns The number of bytes read (musly_track_binsize()) in case of
 * success, -1 in case an error occurred.
 *
 * \note This transforms the musly_track data from network byte order (which is
 * big endian) to host byte order. If you do not need to transmit the data
 * across platforms, you can directly copy musly_track_binsize() bytes from
 * \p from_buffer to \p to_track, or not use a buffer at all.
 *
 * \sa musly_jukebox_poweron(), musly_track_binsize(), musly_track_tobin().
 */
MUSLY_EXPORT int
musly_track_frombin(
        musly_jukebox* jukebox,
        unsigned char* from_buffer,
        musly_track* to_track);


/** This function displays a string representation of the given musly_track.
 * The data is displayed in a flat format. All data structures (matrices,
 * covariance matrices) are exported as vectors. This call can be used to
 * export the feature data for further analysis.
 * Note: This function is not threadsafe!
 *
 * \param[in] jukebox The Musly jukebox to use.
 * \param[in] from_track the musly_track to convert into a string
 * representation.
 *
 * \returns a constant null terminated string representing from_track.
 *
 * \sa musly_track_tobin(), musly_track_frombin()
 */
const char*
musly_track_tostr(
        musly_jukebox* jukebox,
        musly_track* from_track);


/** Compute a music similarity model (musly_track) from the given PCM signal.
 * The audio is analyzed according to the initialized music similarity method
 * and the musly_track is filled with the feature data. The musly_track can
 * then be used to compute the music similarity between other musly_track
 * features (use musly_jukebox_similarity()).
 * If you are analyzing music files, use musly_track_analyze_audiofile() which
 * does the decoding and down-/re-sampling of audio itself.
 *
 * \note
 * Depending on the music similarity method, not all of the given signal will
 * be used in the computation, but possibly only the central 60 seconds.
 *
 * \param[in] jukebox A reference to an initialized musly_jukebox object.
 * \param[in] mono_22khz_pcm The audio signal to analyze represented as a PCM float
 * array. The audio signal has to be mono and sampled at 22050hz with float
 * values between -1.0 and +1.0.
 * \param[in] length_pcm The length of the input float array.
 * \param[out] track The musly_track to write the music similarity features.
 *
 * \returns 0 on success, -1 on failure.
 *
 * \sa musly_track_analyze_audiofile(), musly_jukebox_poweron().
 */
MUSLY_EXPORT int
musly_track_analyze_pcm(
        musly_jukebox* jukebox,
        float* mono_22khz_pcm,
        int length_pcm,
        musly_track* track);


/** Compute a music similarity model (musly_track) from the audio file.
 * The audio file is decoded with the decoder selected when initializing
 * the musly_jukebox, down- and re-sampled to a 22050Hz mono signal before
 * a musly_track is computed. The audio is analyzed according to the
 * initialized music similarity method and the musly_track is filled with the
 * feature data. To compute the similarity to other musly_track objects,
 * use the musly_jukebox_similarity() function. If you already decoded the
 * PCM signal of the music you want to analyze, use musly_track_analyze_pcm().
 *
 * \note
 * While you can control which portion of the file will be decoded and passed
 * to musly_track_analyze_pcm(), it depends on the music similarity measure
 * whether the full excerpt is going to be used to build the similarity model.
 * Generally, it is enough to decode 30 to 60 seconds, and it is advisable to
 * exclude nonrepresentative parts such as the intro and outro of a song.
 *
 * \param[in] jukebox A reference to an initialized musly_jukebox object.
 * \param[in] audiofile An audio file. The file will be decoded with the audio
 * decoder.
 * \param[in] excerpt_length The maximum length in seconds of the excerpt to
 * decode. If zero or greater than the file length, decodes the whole file.
 * Suggested value: 30.
 * \param[in] excerpt_start The starting position in seconds of the excerpt to
 * decode. If zero, decoding starts at the beginning. If negative, the excerpt
 * is centered in the file, but starts at <tt>-excerpt_start<tt> the latest.
 * If positive and <tt>excerpt_start + excerpt_length</tt> exceeds the file
 * length, then the excerpt is taken from the end of the file.
 * Suggested value: -48.
 * \param[out] track The musly_track to write the music similarity features.
 *
 * \returns 0 on success, -1 on failure.
 *
 * \sa musly_track_analyze_audiofile(), musly_jukebox_poweron().
 */
MUSLY_EXPORT int
musly_track_analyze_audiofile(
        musly_jukebox* jukebox,
        const char* audiofile,
        float excerpt_length,
        float excerpt_start,
        musly_track* track);

#ifdef __cplusplus
}
#endif

#endif // MUSLY_H_
