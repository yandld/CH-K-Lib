/*----------------------------------------------------------------------------
 *      RL-ARM - USB
 *----------------------------------------------------------------------------
 *      Name:    usbd_user_desc.c
 *      Purpose: User Provided Override of Descriptors
 *      Rev.:    V4.70
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

//#include <RTL.h>
#include <rl_usb.h>


/* Override of Default HID Report Descriptor                                  */
const U8 USBD_HID_ReportDescriptor[] = {
  HID_UsagePage      ( HID_USAGE_PAGE_GENERIC                 ),
  HID_Usage          ( HID_USAGE_GENERIC_MOUSE                ),
  HID_Collection     ( HID_Application                        ),
    HID_Usage        ( HID_USAGE_GENERIC_POINTER              ),
    HID_Collection   ( HID_Physical                           ),
      HID_UsagePage  ( HID_USAGE_PAGE_BUTTON                  ),
      HID_UsageMin   ( 1                                      ),
      HID_UsageMax   ( 3                                      ),
      HID_LogicalMin ( 0                                      ),
      HID_LogicalMax ( 1                                      ),
      HID_ReportCount( 3                                      ),
      HID_ReportSize ( 1                                      ),
      HID_Input      ( HID_Data | HID_Variable | HID_Absolute ),
      HID_ReportCount( 1                                      ),
      HID_ReportSize ( 5                                      ),
      HID_Input      ( HID_Constant                           ),
      HID_UsagePage  ( HID_USAGE_PAGE_GENERIC                 ),
      HID_Usage      ( HID_USAGE_GENERIC_X                    ),
      HID_Usage      ( HID_USAGE_GENERIC_Y                    ),
      HID_Usage      ( HID_USAGE_GENERIC_WHEEL                ),
      HID_LogicalMin ( (U8)-127                               ),
      HID_LogicalMax ( 127                                    ),
      HID_ReportSize ( 8                                      ),
      HID_ReportCount( 3                                      ),
      HID_Input      ( HID_Data | HID_Variable | HID_Relative ),
    HID_EndCollection,
  HID_EndCollection,
};

const U16 USBD_HID_ReportDescriptorSize = sizeof(USBD_HID_ReportDescriptor);

/* Override of Default USB Configuration Descriptor                           */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor       */
const U8 USBD_ConfigDescriptor[] = {
  /* Configuration 1                                                          */
  USB_CONFIGUARTION_DESC_SIZE,          /* bLength                            */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,    /* bDescriptorType                    */
  WBVAL(                                /* wTotalLength                       */
    USB_CONFIGUARTION_DESC_SIZE +
    USB_INTERFACE_DESC_SIZE     +
    USB_HID_DESC_SIZE           +
    USB_ENDPOINT_DESC_SIZE
  ),
  0x01,                                 /* bNumInterfaces                     */
  0x01,                                 /* bConfigurationValue                */
  0x00,                                 /* iConfiguration                     */
  USB_CONFIG_BUS_POWERED,               /* bmAttributes                       */
  USB_CONFIG_POWER_MA(100),             /* bMaxPower                          */

  /* Interface 0, Alternate Setting 0, HID Class                              */
  USB_INTERFACE_DESC_SIZE,              /* bLength                            */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType                    */
  0x00,                                 /* bInterfaceNumber                   */
  0x00,                                 /* bAlternateSetting                  */
  0x01,                                 /* bNumEndpoints                      */
  USB_DEVICE_CLASS_HUMAN_INTERFACE,     /* bInterfaceClass                    */
  HID_SUBCLASS_BOOT,                    /* bInterfaceSubClass                 */
  HID_PROTOCOL_MOUSE,                   /* bInterfaceProtocol                 */
  0x04,                                 /* iInterface                         */

  /* HID Class Descriptor                                                     */
  USB_HID_DESC_SIZE,                    /* bLength                            */
  HID_HID_DESCRIPTOR_TYPE,              /* bDescriptorType                    */
  WBVAL(0x0100),    /* 1.00 */          /* bcdHID                             */
  0x00,                                 /* bCountryCode                       */
  0x01,                                 /* bNumDescriptors                    */
  HID_REPORT_DESCRIPTOR_TYPE,           /* bDescriptorType                    */
  WBVAL(USB_HID_REPORT_DESC_SIZE),      /* wDescriptorLength                  */

  /* Endpoint, HID Interrupt In                                               */
  USB_ENDPOINT_DESC_SIZE,               /* bLength                            */
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType                    */
  USB_ENDPOINT_IN(1),                   /* bEndpointAddress                   */
  USB_ENDPOINT_TYPE_INTERRUPT,          /* bmAttributes                       */
  WBVAL(0x0004),                        /* wMaxPacketSize                     */
  0x20,             /* 32ms */          /* bInterval                          */

  /* Terminator                                                               */
  0                                     /* bLength                            */
};
