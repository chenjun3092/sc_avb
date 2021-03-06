#ifndef __local_listener_stream_h__
#define __local_listener_stream_h__
#ifndef __XC__
#ifndef streaming
#define streaming
#endif
#ifndef NULLABLE
#define NULLABLE
#endif
#else
#define NULLABLE ?
#endif
#include <xccompat.h>
#include "avb_conf.h"

// A TS packet is 1 word timestamp, 188 bytes, one word 'inuse'
#define TS_OUTPUT_PACKET_SIZE (4+188+4)

// The 61883-4, section 7, recommends this number of packets be stored
#define TS_OUTPUT_FIFO_SIZE 17

// Size of the FIFO in words
#define MEDIA_OUTPUT_FIFO_WORD_SIZE ((TS_OUTPUT_PACKET_SIZE*TS_OUTPUT_FIFO_SIZE)/4)

#define MEDIA_OUTPUT_FIFO_INUSE_OFFSET 48

// This must match the C structure in media_output_fifo.c
struct ofifo_t {
	unsigned packet_wr;								//!< The packet being written
	unsigned fifo[MEDIA_OUTPUT_FIFO_WORD_SIZE];		//!< The fifo of packets
};

/**
 * \brief This type provides the data structure used by a media output FIFO.
 */
typedef struct ofifo_t media_output_fifo_data_t;

/**
 * \brief This type provides a handle to a media output FIFO.
 **/
typedef int media_output_fifo_t;

/**
 * \brief Intiialise a FIFO
 */
void media_output_fifo_init(media_output_fifo_t s, unsigned stream_num);

/**
 * \brief Disable a FIFO
 *
 * This prevents samples from flowing through the FIFO
 */
void disable_media_output_fifo(media_output_fifo_t s);

/**
 * \brief Enable a FIFO
 *
 * This starts samples flowing through the FIFO
 */
void enable_media_output_fifo(media_output_fifo_t s,
                              int media_clock);

/**
 *  \brief Perform maintanance on the FIFO, called periodically
 *
 *  This should be called periodically to allow the FIFO to
 *  perform tasks such as informing the clock recovery thread
 *  of some new timing information.
 *
 *  \param s the fifo to maintain
 *  \param buf_ctl a channel end that links the FIFO to the media clock service
 *  \param notified_buf_ctl pointer to a flag which is set when the media clock has been notified of a timing event in the FIFO
 */
void
media_output_fifo_maintain(media_output_fifo_t s,
                           chanend buf_ctl,
                           REFERENCE_PARAM(int, notified_buf_ctl));


#ifndef __XC__

/**
 *  \brief Push a set of samples into the FIFO
 *
 *  The samples are taken from the buffer pointed to by the sample_ptr,
 *  but are read from that buffer with a stride between each sample.
 *
 *  The 1722 listener thread uses this to put samples from the decoded
 *  packet into the audio FIFOs.
 *
 *  \param s0 the FIFO to push samples into
 *  \param sample_ptr a pointer to a block of samples in the 1722 packet
 *  \param index the index of the first 6 word block to write within the current packet
 *  \param n the number of 6 word blocks to push into the buffer
 */
void
media_output_fifo_push(media_output_fifo_t s0, unsigned int *sample_ptr, int index, int n);
#endif


/**
 *  \brief Handle notification events on the buffer control channel
 *
 *  The 1722 listener thread notifies the clock server about timing
 *  events in the audio FIFOs using a channel. This function provides
 *  the processing of the messages which can be sent by the clock
 *  recovery thread over that channel.
 *
 *  \param buf_ctl  the communication channel with the clock recovery service
 *  \param stream_num  the number of the stream which is being handled
 *  \param buf_ctl_notified pointer to the flag which indicates whether the clock recovery thread has been notified of a timing event
 */
void
media_output_fifo_handle_buf_ctl(chanend buf_ctl,
                                 int stream_num,
                                 REFERENCE_PARAM(int, buf_ctl_notified));


/** Initialize media output FIFOs.
 *
 *  This function initializes media output FIFOs and ties the handles
 *  to their associated data structures. It should be called before the main
 *  component function on a thread to setup the FIFOs.
 *
 *  \param ofifos      an array of media output FIFO handles to initialize
 *  \param ofifo_data  an array of associated data structures
 *  \param n           the number of FIFOs to initialize
 **/
void
init_media_output_fifos(media_output_fifo_t ofifos[],
						media_output_fifo_data_t ofifo_data[],
						int n);

#endif


