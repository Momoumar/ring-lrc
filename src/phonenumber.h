/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#ifndef PHONENUMBER_H
#define PHONENUMBER_H

#include "typedefs.h"

//Qt
#include <QStringList>
#include <QtCore/QSize>
#include <QtCore/QObject>

//SFLPhone
class Account;
class Contact;
class Call;
class PhoneNumberPrivate;

///PhoneNumber: represent a phone number
class LIB_EXPORT PhoneNumber : public QObject {
   Q_OBJECT
public:
   friend class PhoneDirectoryModel;
   virtual ~PhoneNumber(){};

   //Properties
   Q_PROPERTY(Account*      account   READ account  WRITE setAccount)
   Q_PROPERTY(Contact*      contact   READ contact  WRITE setContact)
   Q_PROPERTY(time_t        lastUsed  READ lastUsed                 )
   Q_PROPERTY(QString       uri       READ uri                      )
   Q_PROPERTY(QString       type      READ type                     )
   Q_PROPERTY(int           callCount READ callCount                )
   Q_PROPERTY(QList<Call*>  calls     READ calls                    )

   ///@enum PresenceStatus: Presence status
   enum class PresenceStatus {
      UNTRACKED = 0,
      PRESENT   = 1,
      ABSENT    = 2,
   };
   Q_ENUMS(PresenceStatus)

   ///@enum State: Is this temporary, blank, used or unused
   enum class State {
      BLANK     = 0, //This number represent no number
      TEMPORARY = 1, //This number is not yet complete
      USED      = 2, //This number have been called before
      UNUSED    = 3, //This number have never been called, but is in the address book
   };
   Q_ENUMS(State)

   //Getters
   QString            uri           () const;
   QString            type          () const;
   bool               present       () const;
   QString            presentMessage() const;
   Account*           account       () const;
   Contact*           contact       () const;
   time_t             lastUsed      () const;
   PhoneNumber::State state         () const;
   int                callCount     () const;
   QList<Call*>       calls         () const;

   //Setters
   void setAccount(Account* account);
   void setContact(Contact* contact);

   //Static
   static const PhoneNumber* BLANK;

private:
   friend class PhoneNumberPrivate;

   //Constructor
   PhoneNumber(const QString& uri, const QString& type);

   //Attributes
   QString            m_Uri           ;
   QString            m_Type          ;
   bool               m_Present       ;
   QString            m_PresentMessage;
   bool               m_Tracked       ;
   bool               m_Temporary     ;
   Contact*           m_pContact      ;
   Account*           m_pAccount      ;
   time_t             m_LastUsed      ;
   PhoneNumber::State m_State         ;
   QList<Call*>       m_lCalls        ;

   //Static attributes
   static QHash<int,Call*> m_shMostUsed;

};

Q_DECLARE_METATYPE( PhoneNumber* )

#endif