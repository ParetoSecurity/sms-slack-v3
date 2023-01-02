# sms2slack: Receive SMS messages and forward them to Slack

sms2slack is a tiny box for a SIM card that receives SMS messages, like 2FA codes, and forwards them to your Slack channel.


[![PlatformIO CI](https://github.com/ParetoSecurity/sms-slack-v3/actions/workflows/build.yml/badge.svg)](https://github.com/ParetoSecurity/sms-slack-v3/actions/workflows/build.yml)


## Hardware

You can purchase the LILYGO® TTGO T-SIM A7670E/SA board for GSM A (Americas) or E (Europe) region from [AliExpress](https://www.aliexpress.com/item/1005003036514769.html).

[3D print templates for the box](https://www.printables.com/model/219370-case-for-sms2slack-v2) that fits the AliExpress board.

## Flashing or Updating firmware


[<img width="600" alt="Screenshot 2023-01-02 at 15 29 20" src="https://user-images.githubusercontent.com/1309671/210244823-9f7ca286-ebd3-421a-bf46-89e7c19c164e.png">](https://www.youtube.com/watch?v=TGqXmkX-YNo)

1. Install & update firmware
2. Connect device to the Wi-Fi network
3. Configure Slack URL from [Slack Webhook App](https://slack.com/apps/A0F7XDUAZ-incoming-webhooks?tab=more_info)
4. Access logs and send terminal commands
5. Attach your device and click on **Connect* and select "USB to UART Bridge" in the web browser prompt
6. Click on **Install** or **Update** and follow instructions


### Schematics for SIM7000x + ESP32

![image](https://user-images.githubusercontent.com/239513/187633707-db07fb35-faea-4ff8-bcfa-98055d0f62fa.png)

## FAQ

**What carriers are supported?**

The selected board supports all LTE CAT-1 carriers and has support for 2G networks. We do not recommend using a (usually cheaper) board that supports only 2G/3G as those networks are getting phased out fast.

**How long does it take to put everything together?**

If you're comfortable with electronics and a bit of code, it should be less than an hour.

## Why

#### Phone number Two-Factor Authentication is dangerous and inconvenient

Using Two-Factor Authentication with your phone is annoying when coworkers need the code, but you're on vacation, swimming, surfing, or biking. Using a phone number that is publicly shared opens you to the risk of SIM swap and hijacking of your phone number.

We recommend using authenticator apps over phone numbers where possible. Unfortunately, some websites allow only phone number 2FA. For us, two banks did not provide any other option but phone number 2FA.

sms2slack makes it easy to share the 2FA codes between coworkers and not use someone's personal phone or company's publicly available phone numbers.

If you're a digital nomad, you probably use local SIMs when you're traveling, and it's really annoying to put back your home SIM just to get one 2FA code.

As an added benefit, sms2slack is immune to any spyware or viruses sent to it, which cannot be said for your phone.

#### VoIP and online SMS receiving services do not work for all websites

Our company used Twilio to receive and forward messages to our Slack in the past. Unfortunately, not all websites send messages to these phone numbers. Most notably, Apple, Microsoft, Google, Facebook, and WhatsApp do not, and many of them now require 2FA for business use, and often the only option is a phone number.

There are many other services like Twilio, but they all have this issue. The only option is to have an actual phone number connected to a device. With sms2slack you can use local telecom SIM cards which, from our experience, never have issues receiving messages.
