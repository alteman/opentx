/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "progresswidget.h"
#include "progressdialog.h"
#include "ui_progressdialog.h"
#include "appdata.h"
#include <QTimer>

ProgressDialog::ProgressDialog(QWidget *parent, const QString &title, const QIcon &icon, bool forceOpen):
QDialog(parent),
ui(new Ui::ProgressDialog),
locked(false)
{
  ui->setupUi(this);
  setWindowTitle(title);
  setWindowIcon(icon);
  if (forceOpen) {
    ui->outputProgress->forceOpen();
  }
  resize(0, 0);
  show();
}

ProgressDialog::~ProgressDialog()
{
  delete ui;
}

ProgressWidget * ProgressDialog::progress()
{
  return ui->outputProgress;
}

void ProgressDialog::on_closeButton_clicked()
{
  if (!locked) {
    ui->outputProgress->stop();
    close();
  }
}

void ProgressDialog::on_outputProgress_detailsToggled()
{
  QTimer::singleShot(0, this, SLOT(shrink()));
}

void ProgressDialog::on_outputProgress_locked(bool lock)
{
  ui->closeButton->setEnabled(!lock);
  locked = lock;
}

void ProgressDialog::shrink()
{
  resize(0, 0);
}

bool ProgressDialog::isEmpty() const
{
  return ui->outputProgress->isEmpty();
}
