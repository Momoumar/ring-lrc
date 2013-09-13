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
#include "phonedirectorymodel.h"

//Qt
#include <QtCore/QCoreApplication>

//SFLPhone
#include "phonenumber.h"
#include "call.h"
#include "account.h"
#include "contact.h"

PhoneDirectoryModel* PhoneDirectoryModel::m_spInstance = nullptr;

PhoneDirectoryModel::PhoneDirectoryModel(QObject* parent) :
   QAbstractTableModel(parent?parent:QCoreApplication::instance())
{
   setObjectName("PhoneDirectoryModel");
}

PhoneDirectoryModel* PhoneDirectoryModel::instance()
{
   if (!m_spInstance) {
      m_spInstance = new PhoneDirectoryModel();
   }
   return m_spInstance;
}

QVariant PhoneDirectoryModel::data(const QModelIndex& index, int role ) const
{
   if (!index.isValid()) return QVariant();
   const PhoneNumber* number = m_lNumbers[index.row()];
   switch (static_cast<PhoneDirectoryModel::Columns>(index.column())) {
      case PhoneDirectoryModel::Columns::URI:
         switch (role) {
            case Qt::DisplayRole:
               return number->uri();
               break;
         }
      case PhoneDirectoryModel::Columns::TYPE:
         switch (role) {
            case Qt::DisplayRole:
               return number->type();
               break;
         }
      case PhoneDirectoryModel::Columns::CONTACT:
         switch (role) {
            case Qt::DisplayRole:
//                return QVariant::fromValue(const_cast<Contact*>(number->contact()));
               return number->contact()?number->contact()->formattedName():QVariant();
               break;
         }
      case PhoneDirectoryModel::Columns::ACCOUNT:
         switch (role) {
            case Qt::DisplayRole:
//                return QVariant::fromValue(const_cast<Account*>(number->account()));
               return number->account()?number->account()->id():QVariant();
               break;
         }
      case PhoneDirectoryModel::Columns::STATE:
         switch (role) {
            case Qt::DisplayRole:
               return (int)number->state();
               break;
         }
      case PhoneDirectoryModel::Columns::CALL_COUNT:
         switch (role) {
            case Qt::DisplayRole:
               return number->callCount();
               break;
         }
      case PhoneDirectoryModel::Columns::LAST_USED:
         switch (role) {
            case Qt::DisplayRole:
               return (int)number->lastUsed();
               break;
         }
   }
   return QVariant();
}

int PhoneDirectoryModel::rowCount(const QModelIndex& parent ) const
{
   if (parent.isValid())
      return 0;
   return m_lNumbers.size();
}

int PhoneDirectoryModel::columnCount(const QModelIndex& parent ) const
{
   Q_UNUSED(parent)
   return 7;
}

Qt::ItemFlags PhoneDirectoryModel::flags(const QModelIndex& index ) const
{
   Q_UNUSED(index)
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

///This model is read and for debug purpose
bool PhoneDirectoryModel::setData(const QModelIndex& index, const QVariant &value, int role )
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QVariant PhoneDirectoryModel::headerData(int section, Qt::Orientation orientation, int role ) const
{
   Q_UNUSED(section)
   Q_UNUSED(orientation)
   constexpr static const char* headers[] = {"URI", "Type", "Contact", "Account", "State", "Call count", "Last used"};
   if (role == Qt::DisplayRole) return headers[section];
   return QVariant();
}

PhoneNumber* PhoneDirectoryModel::getNumber(const QString& uri, const QString& type)
{
   NumberWrapper* wrap = m_hDirectory[uri];
   if (wrap)
      return wrap->numbers[0];

   //Too bad, lets create one
   PhoneNumber* number = new PhoneNumber(uri,type);
   m_lNumbers << number;
   emit layoutChanged();
   wrap = new NumberWrapper();
   wrap->numbers << number;
   m_hDirectory[uri] = wrap;
   return number;
}

PhoneNumber* PhoneDirectoryModel::getNumber(const QString& uri, Account* account, const QString& type)
{
   //See if the number is already loaded
   NumberWrapper* wrap = m_hDirectory[uri];
   foreach(PhoneNumber* number, wrap->numbers) {
      //Not perfect, but better than ignoring the high probabilities
      if (!number->account())
         number->setAccount(account);
      if (number->account() == account)
         return number;
   }

   //Create the number
   PhoneNumber* number = new PhoneNumber(uri,type);
   number->setAccount(account);
   m_lNumbers << number;
   wrap = new NumberWrapper();
   wrap->numbers << number;
   m_hDirectory[uri] = wrap;
   emit layoutChanged();
   return number;
}

PhoneNumber* PhoneDirectoryModel::getNumber(const QString& uri, Contact* contact, Account* account, const QString& type)
{
   //See if the number is already loaded
   NumberWrapper* wrap = m_hDirectory[uri];
   if (wrap) {
      foreach(PhoneNumber* number, wrap->numbers) {
         if (number->contact() == contact && number->account() == account) {
            qDebug() << "\n\nRETURN" << uri << number->uri();
            return number;
         }
      }
   }

   //Create the number
   PhoneNumber* number = new PhoneNumber(uri,type);
   number->setAccount(account);
   number->setContact(contact);
   m_lNumbers << number;
   wrap = new NumberWrapper();
   wrap->numbers << number;
   m_hDirectory[uri] = wrap;
   emit layoutChanged();
   return number;
}

PhoneNumber* PhoneDirectoryModel::getTemporaryNumber(const QString& uri, const QString& type)
{
   PhoneNumber* number = getNumber(uri,type);
   if (number)
      return number;
   NumberWrapper* wrap = m_hTemporaryNumbers[uri];
   if (wrap)
      return wrap->numbers[0];
   return getNumber(uri,type);//TODO
}

PhoneNumber* PhoneDirectoryModel::getTemporaryNumber(const QString& uri, Account* account, const QString& type)
{
   return getNumber(uri,account,type);
}

PhoneNumber* PhoneDirectoryModel::getTemporaryNumber(const QString& uri, Contact* contact, Account* account, const QString& type)
{
   return getNumber(uri,contact,account,type);
}