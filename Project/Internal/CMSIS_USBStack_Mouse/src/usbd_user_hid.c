#include <rl_usb.h>

extern void GetMouseInReport (S8 *report, U32 size); /* Mouse Function        */

U8 feat;

void usbd_hid_init (void) {}

int usbd_hid_get_report (U8 rtype, U8 rid, U8 *buf, U8 req) {
  U32 i;
  S8 report[4] = {0, 0, 0, 0};

  switch (rtype) {
    case HID_REPORT_INPUT:
      switch (rid) {
         case 0:
          switch (req) {
            case USBD_HID_REQ_EP_CTRL:
            case USBD_HID_REQ_PERIOD_UPDATE:
              GetMouseInReport (report, 4);
              for (i = 0; i < sizeof(report); i++) {
                *buf++ = (U8)report[i];
              }
              return (1);
            case USBD_HID_REQ_EP_INT:
              break;
          }
           break;
      }
      break;
    case HID_REPORT_FEATURE:
      buf[0] = feat;
      return (1);
  }
  return (0);
}

void usbd_hid_set_report (U8 rtype, U8 rid, U8 *buf, int len, U8 req) {

  switch (rtype) {
    case HID_REPORT_OUTPUT:
      break;
    case HID_REPORT_FEATURE:
      break;
  }
}