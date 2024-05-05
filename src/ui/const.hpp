#ifndef CONST_HPP
#define CONST_HPP

const char* CUSTOM_VERTICAL_SCROLL_BAR = "QScrollBar:vertical {"
                                         "    border: 1px solid #999999;"
                                         "    background: rgba(255, 255, 255, 0);"
                                         "    width: 15px;"
                                         "    margin: 0px 0px 0px 0px;"
                                         "    border: none;"
                                         "}"
                                         "QScrollBar::handle:vertical {"
                                         "    background: rgba(102, 102, 102, 30%);"
                                         "    min-height: 20px;"
                                         "    border-radius: 10px;"
                                         "}"
                                         "QScrollBar::add-line:vertical {"
                                         "    border: none;"
                                         "    background: rgba(224, 224, 224, 0);"
                                         "}"
                                         "QScrollBar::sub-line:vertical {"
                                         "    border: none;"
                                         "    background: rgba(224, 224, 224, 0);"
                                         "}";

const char* CUSTOM_HORIZONTAL_SCROLL_BAR = "QScrollBar:horizontal {"
                                           "    border: 1px solid #999999;"
                                           "    background: rgba(255, 255, 255, 0);"
                                           "    height: 15px;"
                                           "    margin: 0px 0px 0px 0px;"
                                           "    border: none;"
                                           "}"
                                           "QScrollBar::handle:horizontal {"
                                           "    background: rgba(102, 102, 102, 30%);"
                                           "    min-width: 20px;"
                                           "    border-radius: 10px;"
                                           "}"
                                           "QScrollBar::add-line:horizontal {"
                                           "    border: none;"
                                           "    background: rgba(224, 224, 224, 0);"
                                           "}"
                                           "QScrollBar::sub-line:horizontal {"
                                           "    border: none;"
                                           "    background: rgba(224, 224, 224, 0);"
                                           "}";

#endif
