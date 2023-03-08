#include "drivers/tty/ldisc.h"
#include <drivers/keyboard.h>
#include <drivers/tty/tty.h>
#include <errno.h>
#include <util/bits.h>
#include <util/debug.h>
#include <util/string.h>
#include <drivers/tty/vterminal.h>

#define ldisc_to_tty(ldisc) CONTAINER_OF((ldisc), tty_t, tty_ldisc)

/**
 * Initialize the line discipline. Don't forget to wipe the buffer associated
 * with the line discipline clean.
 *
 * @param ldisc line discipline.
 */
void ldisc_init(ldisc_t *ldisc)
{
    ldisc->ldisc_cooked=0; // cooked == tail == head when empty
    ldisc->ldisc_tail=0;
    ldisc->ldisc_head=0;
    ldisc->ldisc_full=0; // Not NULL
    sched_queue_init(&ldisc->ldisc_read_queue); // Initialize read queue
    memset(ldisc->ldisc_buffer,'\0',LDISC_BUFFER_SIZE); // Clean the buffer
    NOT_YET_IMPLEMENTED("DRIVERS: ldisc_init");
}

/**
 * While there are no new characters to be read from the line discipline's
 * buffer, you should make the current thread to sleep on the line discipline's
 * read queue. Note that this sleep can be cancelled. What conditions must be met 
 * for there to be no characters to be read?
 *
 * @param  ldisc the line discipline
 * @param  lock  the lock associated with `ldisc`
 * @return       0 if there are new characters to be read or the ldisc is full.
 *               If the sleep was interrupted, return what
 *               `sched_cancellable_sleep_on` returned (i.e. -EINTR)
 */
long ldisc_wait_read(ldisc_t *ldisc, spinlock_t *lock)
{
    // Condition: When there are no cooked characters in buffer
    long ret;
    while(ldisc->ldisc_cooked==ldisc->ldisc_tail){
      ret=sched_cancellable_sleep_on(&ldisc->ldisc_read_queue,lock); // Put current thread into sleep
    }   
    // NOT_YET_IMPLEMENTED("DRIVERS: ldisc_wait_read");
    return ret;
}

/**
 * Reads `count` bytes (at max) from the line discipline's buffer into the
 * provided buffer. Keep in mind the the ldisc's buffer is circular.
 *
 * If you encounter a new line symbol before you have read `count` bytes, you
 * should stop copying and return the bytes read until now. We also need to put it into buf
 * 
 * If you encounter an `EOT` you should stop reading and you should NOT include 
 * the `EOT` in the count of the number of bytes read
 *
 * @param  ldisc the line discipline
 * @param  buf   the buffer to read into.
 * @param  count the maximum number of bytes to read from ldisc.
 * @return       the number of bytes read from the ldisc.
 */
size_t ldisc_read(ldisc_t *ldisc, char *buf, size_t count)
{
    // TODO: Find a way to lock the buffer
    size_t cur_count=0; // Compute the current count in provided buffer buf

    // If the ldisc_cooked didn't reach the end of the buffer
    int j=0;
    if(ldisc->ldisc_cooked>ldisc->ldisc_tail&&ldisc->ldisc_cooked<LDISC_BUFFER_SIZE){
    while(ldisc->ldisc_tail!=ldisc->ldisc_cooked){
        if(ldisc->ldisc_buffer[ldisc->ldisc_tail]==EOT){
            ldisc->ldisc_tail++;
            return cur_count; // Stop reading and return
        }
        if(ldisc->ldisc_buffer[ldisc->ldisc_tail]=='\n'){
            // TODO: It means that discard the reamaining element between i and cooked?
            buf[j]=ldisc->ldisc_buffer[ldisc->ldisc_tail];
            ldisc->ldisc_tail++;          
            cur_count++;
            return cur_count; // When encounter a new line symbol
        }
        buf[j]=ldisc->ldisc_buffer[ldisc->ldisc_tail];
        j++;
        ldisc->ldisc_tail++;
        cur_count++;
        if(cur_count==count){ // If it has reach count bytes
            return count;           
        }
    }
}
    else if(ldisc->ldisc_cooked<ldisc->ldisc_tail){
        // Put the element from tail to end into provided buffer firstly
        for(size_t i=ldisc->ldisc_tail;i<LDISC_BUFFER_SIZE;i++){ 
            if(ldisc->ldisc_buffer[i]==EOT){
                return cur_count; // Stop reading and return
            }
            if(ldisc->ldisc_buffer[i]=='\n'){
                buf[j]=ldisc->ldisc_buffer[ldisc->ldisc_tail];
                ldisc->ldisc_tail++;          
                cur_count++;
                return cur_count; // When encounter a new line symbol
            }
            buf[j]=ldisc->ldisc_buffer[ldisc->ldisc_tail];
            j++;
            ldisc->ldisc_tail++;
            cur_count++;
            if(cur_count==count){ // If it has reach count bytes
                return count;           
            }
        }
        // Then put the element from the begining to the cooked element
        for(size_t i=0;i<ldisc->ldisc_cooked;i++){
            if(ldisc->ldisc_buffer[i]==EOT){
               return cur_count; // Stop reading and return
            }
            if(ldisc->ldisc_buffer[i]=='\n'){
                //ldisc->ldisc_tail=ldisc->ldisc_cooked; // Update tail
                //ldisc->ldisc_cooked=ldisc->ldisc_head;
                //sched_broadcast_on(&ldisc->ldisc_read_queue); 
                buf[j]=ldisc->ldisc_buffer[ldisc->ldisc_tail];
                ldisc->ldisc_tail++;          
                cur_count++;
                return cur_count; // When encounter a new line symbol
            }
            buf[j]=ldisc->ldisc_buffer[ldisc->ldisc_tail];
            j++;
            ldisc->ldisc_tail++;
            cur_count++;
            if(cur_count==count){ // If it has reach count bytes
                return count;           
            }          
        }
    }
      // NOT_YET_IMPLEMENTED("DRIVERS: ldisc_read");
    return cur_count;
    }


/**
 * Place the character received into the ldisc's buffer. You should also update
 * relevant fields of the struct.
 *
 * An easier way of handling new characters is making sure that you always have
 * one byte left in the line discipline. This way, if the new character you
 * received is a new line symbol (user hit enter), you can still place the new
 * line symbol into the buffer; if the new character is not a new line symbol,
 * you shouldn't place it into the buffer so that you can leave the space for
 * a new line symbol in the future. 
 * 
 *
 * If the line discipline is almost full, i.e. the case where there is one slot 
 * left in the line discipline buffer, the only characters that can be handled are:
 * EOT --> ctrl-D
 * \n --> newline or LF 
 * ETX --> ctrl-C
 * \b --> backspace
 * Here are some special cases to consider:
 *      1. If the character is a backspace:
 *          * if there is a character to remove you must also emit a `\b` to
 *            the vterminal.
 *      2. If the character is end of transmission (EOT) character (typing ctrl-d)
 *      3. If the character is end of text (ETX) character (typing ctrl-c)
 *      4. If your buffer is almost full and what you received is not a new line
 *      symbol
 *
 * If you did receive a new line symbol, you should wake up the thread that is
 * sleeping on the wait queue of the line discipline. You should also
 * emit a `\n` to the vterminal by using `vterminal_write`.  
 * 
 * If you encounter the `EOT` character, you should add it to the buffer, 
 * cook the buffer, and wake up the reader (but do not emit an `\n` character 
 * to the vterminal)
 * 
 * In case of `ETX` you should cause the input line to be effectively transformed
 * into a cooked blank line. You should clear uncooked portion of the line, by 
 * adjusting ldisc_head. 
 * Clarification:In the case of an ETX, you would want to remove the uncooked part and 
 * have a '\n' written on the terminal.On the terminal, the cursor should just move onto 
 * the next line without executing anything that was typed before.
 *
 * Finally, if the none of the above cases apply you should fallback to
 * `vterminal_key_pressed`.
 *
 * Don't forget to write the corresponding characters to the virtual terminal
 * when it applies!
 *
 * @param ldisc the line discipline
 * @param c     the new character
 */
void ldisc_key_pressed(ldisc_t *ldisc, char c)
{
    if((ldisc->ldisc_tail-ldisc->ldisc_head)==1){ // If there are only one byte left in the line discipline
        if (c==EOT) {  // If we encounter Ctrl+D
            ldisc->ldisc_buffer[ldisc->ldisc_head]=c; // Put EOT into buffer
            ldisc->ldisc_head++;
            ldisc->ldisc_full=1; // Mark it as full 
            ldisc->ldisc_cooked=ldisc->ldisc_head; // Cook the buffer
            sched_wakeup_on(&ldisc->ldisc_read_queue,NULL);
        }
        else if (c==ETX) { // We need to discard all the element we put into buffer
            if(ldisc->ldisc_head<ldisc->ldisc_cooked){ // If head has beyond the end of buffer
                for(int i=ldisc->ldisc_cooked;i<LDISC_BUFFER_SIZE;i++){
                    ldisc->ldisc_buffer[i]='\0'; // Clear the raw part 1
                }
                for(int i=0;i<ldisc->ldisc_head;i++){
                    ldisc->ldisc_buffer[i]='\0'; // Clear the raw part 2  
                }
            }
            else{
                for(int i=ldisc->ldisc_cooked;i<ldisc->ldisc_head;i++){ // If the head didn't beyond the end of buffer
                    ldisc->ldisc_buffer[i]='\0'; // Clear the raw part 1
                }
            }
            ldisc->ldisc_head=ldisc->ldisc_cooked; // Discard the raw item
            vterminal_write(&ldisc_to_tty(ldisc)->tty_vterminal,"\n",1); // Output \n to the terminal
        }
        else if(c=='\n'){ // When we encounter next line
            ldisc->ldisc_buffer[ldisc->ldisc_head]=c;
            ldisc->ldisc_head++;
            ldisc->ldisc_full=1; // Mark it as full 
            ldisc->ldisc_cooked=ldisc->ldisc_head; // Set it as cooked
            sched_wakeup_on(&ldisc->ldisc_read_queue,NULL);
            vterminal_write(&ldisc_to_tty(ldisc)->tty_vterminal,"\n",1);
        }
        else if(c=='\b'){ // When we encounter backspace
            // If the head has move to the first position in the buffer and the raw content is not empty
            if(ldisc->ldisc_head==0 && ldisc->ldisc_head!=ldisc->ldisc_cooked){ 
            //ldisc->ldisc_buffer[LDISC_BUFFER_SIZE-1]='\0'; // Clear the last character we input
            ldisc->ldisc_head=LDISC_BUFFER_SIZE-1; // Update the value of head
            // TODO: Do we need to delete characters in terminal
            }
            else if(ldisc->ldisc_head!=ldisc->ldisc_cooked){ // Ensure the raw part is not empty
                //ldisc->ldisc_buffer[ldisc->ldisc_head-1]='\0';
                ldisc->ldisc_head--;
            }
            //strcat(ldisc->ldisc_buffer,'\b')
            //ldisc->ldisc_full=1; // Mark it as full 
            vterminal_write(&ldisc_to_tty(ldisc)->tty_vterminal,"\b",1); // Write '\b' to terminal
        }
    } else if(ldisc->ldisc_full==1){ 
       if(c=='\b'){ // When we encounter backspace
            // If the head has move to the first position in the buffer and the raw content is not empty
            if(ldisc->ldisc_head==0 && ldisc->ldisc_head!=ldisc->ldisc_cooked){ 
                ldisc->ldisc_buffer[LDISC_BUFFER_SIZE-1]='\0'; // Clear the last character we input
                ldisc->ldisc_head=LDISC_BUFFER_SIZE-1; // Update the value of head
                ldisc->ldisc_full=0;
                // TODO: Do we need to delete characters in terminal
            }
        } else if(ldisc->ldisc_head!=ldisc->ldisc_cooked) { // Ensure the raw part is not empty
            ldisc->ldisc_buffer[ldisc->ldisc_head-1]='\0';
            ldisc->ldisc_head--;
            ldisc->ldisc_full=0;
        }      
        //strcat(ldisc->ldisc_buffer,'\b');
        //ldisc->ldisc_full=1; // Mark it as full 
        vterminal_write(&ldisc_to_tty(ldisc)->tty_vterminal,"\b",1); // Write '\b' to terminal
    } else{
        if(c=='\n'){ // When we encounter next line
            ldisc->ldisc_buffer[ldisc->ldisc_head]=c;
            ldisc->ldisc_head++;
            ldisc->ldisc_cooked=ldisc->ldisc_head; // Set it as cooked
            sched_wakeup_on(&ldisc->ldisc_read_queue,NULL);
            vterminal_write(&ldisc_to_tty(ldisc)->tty_vterminal,"\n",1);
            return;
        }
        else if (c==EOT) {  // If we encounter Ctrl+D
            ldisc->ldisc_buffer[ldisc->ldisc_head]=c; // Put EOT into buffer
            ldisc->ldisc_head++;
            ldisc->ldisc_cooked=ldisc->ldisc_head; // Cook the buffer
            sched_wakeup_on(&ldisc->ldisc_read_queue,NULL);
        }
        else if (c==ETX) { // We need to discard all the element we put into buffer
            if(ldisc->ldisc_head<ldisc->ldisc_cooked){ // If head has beyond the end of buffer
                for(int i=ldisc->ldisc_cooked;i<LDISC_BUFFER_SIZE;i++){
                    ldisc->ldisc_buffer[i]='\0'; // Clear the raw part 1
                }
                for(int i=0;i<ldisc->ldisc_head;i++){
                    ldisc->ldisc_buffer[i]='\0'; // Clear the raw part 2  
                }
            }
            else{
                for(int i=ldisc->ldisc_cooked;i<ldisc->ldisc_head;i++){ // If the head didn't beyond the end of buffer
                    ldisc->ldisc_buffer[i]='\0'; // Clear the raw part 1
                }
            }
            ldisc->ldisc_head=ldisc->ldisc_cooked; // Discard the raw item
            vterminal_write(&ldisc_to_tty(ldisc)->tty_vterminal,"\n",1); // Output \n to the terminal
        }
        else if(c=='\b'){ // When we encounter backspace
            // If the head has move to the first position in the buffer and the raw content is not empty
            if(ldisc->ldisc_head==0 && ldisc->ldisc_head!=ldisc->ldisc_cooked){ 
            //ldisc->ldisc_buffer[LDISC_BUFFER_SIZE-1]='\0'; // Clear the last character we input
            ldisc->ldisc_head=LDISC_BUFFER_SIZE-1; // Update the value of head
            // TODO: Do we need to delete characters in terminal
            }
            else if(ldisc->ldisc_head!=ldisc->ldisc_cooked){ // Ensure the raw part is not empty
                //ldisc->ldisc_buffer[ldisc->ldisc_head-1]='\0';
                ldisc->ldisc_head--;
            }
            // strcat(ldisc->ldisc_buffer,'\b')
            // ldisc->ldisc_full=1; // Mark it as full 
            vterminal_write(&ldisc_to_tty(ldisc)->tty_vterminal,"\b",1); // Write '\b' to terminal
        }
        ldisc->ldisc_buffer[ldisc->ldisc_head]=c;
        ldisc->ldisc_head++;
        vterminal_key_pressed(&ldisc_to_tty(ldisc)->tty_vterminal);
    }
    //strcat(ldisc->ldisc_buffer,&c); // Add c into buffer of ldisc
    NOT_YET_IMPLEMENTED("DRIVERS: ldisc_key_pressed");
}

/**
 * Copy the raw part of the line discipline buffer into the buffer provided.
 *
 * @param  ldisc the line discipline
 * @param  s     the character buffer to write to
 * @return       the number of bytes copied
 */
size_t ldisc_get_current_line_raw(ldisc_t *ldisc, char *s)
{
    size_t cur_count=0;
    if(ldisc->ldisc_head<ldisc->ldisc_cooked){
       for(int i=ldisc->ldisc_cooked;i<LDISC_BUFFER_SIZE;i++){
          s[i]=ldisc->ldisc_buffer[i];
          //strcat(s,ldisc->ldisc_buffer[i]); // Add it into character s
          cur_count++;
       }
       for(int i=0;i<ldisc->ldisc_head;i++){
          s[i]=ldisc->ldisc_buffer[i]; // Add it into character s
          cur_count++;
       }
    }
    else{
       for(int i=ldisc->ldisc_cooked;i<ldisc->ldisc_head;i++){
          s[i]=ldisc->ldisc_buffer[i]; // Add it into character s
          cur_count++;
       }
    }
   // NOT_YET_IMPLEMENTED("DRIVERS: ldisc_get_current_line_raw");
    return cur_count;
}
