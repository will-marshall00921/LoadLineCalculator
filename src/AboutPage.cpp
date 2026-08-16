// LoadLineCalculator -- AboutPage.cpp
// Author: Will Marshall
// Created: 8/13/2026, 9:14:04 PM

#include "AboutPage.hpp"
#include <QtCore/QTextStream>

AboutPage::AboutPage(QWidget* parent)
  : QDialog { parent }
  , topLevelLayout { nullptr }
  , scrollArea { nullptr }
  , scrollAreaContentsWidget { nullptr }
  , scrollAreaLayout { nullptr }
  , aboutPageContents { nullptr } {
  setupUi();
  aboutPageContents->setText(generateContents());
}

AboutPage::~AboutPage() noexcept {
  if (scrollArea != nullptr) { delete scrollArea; }
  if (scrollAreaContentsWidget != nullptr) { delete scrollAreaContentsWidget; }
  if (scrollAreaLayout != nullptr) { delete scrollAreaLayout; }
  if (aboutPageContents != nullptr) { delete aboutPageContents; }
  if (topLevelLayout != nullptr) { delete topLevelLayout; }
}

QString AboutPage::generateContents() {
  QString contents;
  QTextStream contentsStream(&contents);
  contentsStream
    << "# LoadLineCalculator\n"
    << "## About\n\n"
    << "A basic load-line calculator and vacuum tube database. Easily completes\n"
    << "most of the common vacuum tube gain stage calculations including headroom\n"
    << "calculations.\n\n"
    << "### Version\n\n"
    << LLC_VERSION << ", " << LLC_VERSION_DATE << "\n\n"
    << "### Author\n\n"
    << "Will Marshall\n";
  contentsStream.flush();
  return contents;
}

void AboutPage::setupUi() {
  if (this->objectName().isEmpty()) {
    setObjectName("aboutPage");
  }
  topLevelLayout = new QVBoxLayout(this);
  topLevelLayout->setObjectName("topLevelLayout");
  scrollArea = new QScrollArea(this);
  scrollArea->setObjectName("scrollArea");
  scrollArea->setVerticalScrollBarPolicy(
    Qt::ScrollBarPolicy::ScrollBarAsNeeded
  );
  scrollArea->setHorizontalScrollBarPolicy(
    Qt::ScrollBarPolicy::ScrollBarAsNeeded
  );
  scrollArea->setWidgetResizable(true);
  scrollAreaContentsWidget = new QWidget();
  scrollAreaContentsWidget->setObjectName("scrollAreaContentsWidget");
  scrollAreaLayout = new QVBoxLayout(scrollAreaContentsWidget);
  scrollAreaLayout->setObjectName("scrollAreaLayout");
  aboutPageContents = new QLabel(scrollAreaContentsWidget);
  aboutPageContents->setObjectName("aboutPageContents");
  aboutPageContents->setTextFormat(
    Qt::TextFormat::MarkdownText
  );
  aboutPageContents->setAlignment(
    Qt::AlignmentFlag::AlignLeading 
      | Qt::AlignmentFlag::AlignLeft
      | Qt::AlignmentFlag::AlignTop
  );
  aboutPageContents->setWordWrap(true);
  scrollAreaLayout->addWidget(aboutPageContents);
  scrollArea->setWidget(scrollAreaContentsWidget);
  topLevelLayout->addWidget(scrollArea);
  
  aboutPageContents->setText(generateContents());
}