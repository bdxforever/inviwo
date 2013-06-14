#include <inviwo/qt/editor/mappingwidget.h>

namespace inviwo {

MappingWidget::MappingWidget(QWidget* parent) : InviwoDockWidget(tr("Mapping"), parent), VoidObserver() {
    setObjectName("MappingWidget");
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);   
	processorNetwork_ = InviwoApplication::getPtr()->getProcessorNetwork();
	eventPropertyManager_ = new EventPropertyManager();

    addObservation(processorNetwork_);
    processorNetwork_->addObserver(this);             
	
    frame_ = new QFrame();
	botLayout_ = new QVBoxLayout();
	comboBox_ = new QComboBox();
	label_ = new QLabel();
	QVBoxLayout* mainLayout = new QVBoxLayout();
	QVBoxLayout* topLayout = new QVBoxLayout();
	QScrollArea* scrollArea = new QScrollArea();
	QWidget* area = new QWidget();

	topLayout->addWidget(label_);
	topLayout->addWidget(comboBox_);
	area->setLayout(botLayout_);
	scrollArea->setWidget(area);
	scrollArea->setWidgetResizable(true);
	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(scrollArea);
	frame_->setLayout(mainLayout);
	setWidget(frame_);

	currentIndex_ = 0;
	QObject::connect(comboBox_, SIGNAL(activated(int)), this, SLOT(comboBoxChange()));

    updateWidget();
}

MappingWidget::~MappingWidget() {}

void MappingWidget::notify() {
    updateWidget();      
}

void MappingWidget::updateWidget() {
	if (comboBox_->count() > 0)	{
		currentIndex_ =  comboBox_->currentIndex();
	}

	curProcessorList_ = processorNetwork_->getProcessors();
	comboBox_->clear();

	std::vector<EventProperty*> eventProperties, tmp;
	std::vector<InteractionHandler*> interactionHandlers;
	std::map<std::string, std::vector<EventProperty*>> eventPropertyMap;
	PropertyOwner* eventPropertyOwner;

	// Get all eventproperties from the processornetwork
	for (size_t i = 0; i < curProcessorList_.size(); ++i) {
		if (curProcessorList_.at(i)->hasInteractionHandler()) {			
			interactionHandlers = curProcessorList_.at(i)->getInteractionHandlers();
			for (size_t j = 0; j < interactionHandlers.size(); ++j) {
				eventPropertyOwner = dynamic_cast<PropertyOwner*>(interactionHandlers.at(j));
				if (eventPropertyOwner) /* Check if interactionhandlar has properties */{
					tmp = eventPropertyOwner->getPropertiesByType<EventProperty>();
					eventProperties.insert(eventProperties.end(), tmp.begin(), tmp.end());
				}
			}
			// Add vector of eventproperties to map with processor identifier as key
			eventPropertyMap[curProcessorList_.at(i)->getIdentifier()] = eventProperties;
			comboBox_->addItem(curProcessorList_.at(i)->getIdentifier().c_str());
			eventProperties.clear();
		}
	}

	if (currentIndex_ > comboBox_->count()-1) currentIndex_ = 0;
	
	label_->setText("Size of processor network: " + intToQString(curProcessorList_.size()));	
	eventPropertyManager_->setEventPropertyMap(eventPropertyMap);
	if (!botLayout_->isEmpty()) emptyLayout(botLayout_); // Clear layout of widgets
	
	// Draw eventpropertywidgets
	if (comboBox_->count() > 0) {
		comboBox_->setCurrentIndex(currentIndex_);
		drawEventPropertyWidgets(); 
	}
}

// Draw the widgets for all EventProperties in EventPropertyManager
void MappingWidget::drawEventPropertyWidgets() {	

	std::string identifier = (const char *)comboBox_->currentText().toAscii();
	eventPropertyManager_->setActiveProcessor(identifier.c_str());
	std::vector<EventProperty*> properties = eventPropertyManager_->getEventPropertiesFromMap();
	
	for (size_t i=0; i<properties.size(); i++) {
		EventProperty* curProperty = properties[i];
		PropertyWidgetQt* propertyWidget = PropertyWidgetFactoryQt::getRef().create(curProperty);
		botLayout_->addWidget(propertyWidget);
		curProperty->registerPropertyWidget(propertyWidget);
		dynamic_cast<EventPropertyWidgetQt*>(propertyWidget)->setManager(eventPropertyManager_);
	}
}

// Clears the layout of all widgets
void MappingWidget::emptyLayout(QVBoxLayout* layout) {
	QObject::disconnect(this, SLOT(comboBoxChange()));
	while(!layout->layout()->isEmpty()) {
		QWidget* w =  layout->layout()->takeAt(0)->widget();
		layout->layout()->removeWidget(w);
		delete w;
	}
}

// For testing
QString MappingWidget::intToQString(int num ) {
    std::stringstream convert;
    convert << num;
    return convert.str().c_str();
}

void MappingWidget::comboBoxChange() {
	emptyLayout(botLayout_);
	drawEventPropertyWidgets();
	//updateWidget();
}

} // namespace